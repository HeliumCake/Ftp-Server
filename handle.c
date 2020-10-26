#include "handle.h"
#include "server.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void ftp_user(Command *cmd, int connfd, int *state)
{
	if (strcmp(cmd->arg, "anonymous") == 0)
	{
		char *reply = "331 Please PASS.\r\n";
		*state = 1;
		m_write(connfd, reply, strlen(reply));
	}
	else
	{
		char *reply = "530 Acessess denied.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
}

void ftp_pass(Command *cmd, int connfd, int *state)
{
	char *reply = "230 Login successfully.\r\n";
	*state = 1;
	m_write(connfd, reply, strlen(reply));
}

void ftp_retr(Command *cmd, int connfd, int datafd, char *dir)
{
	char filename[200];
	strcpy(filename, dir);
	strcat(filename, "/");
	strcat(filename, cmd->arg);
	printf("filename:%s\n", filename);
	int fd = open(filename, O_RDONLY);
	char buffer[100];
	int size;
	while ((size = read(fd, buffer, 100)) > 0)
	{
		if (m_write(datafd, buffer, size) < 0)
		{
			char *reply = "426 Connection closed.\r\n";
			m_write(connfd, reply, strlen(reply));
			close(fd);
			return;
		}
	}
	close(fd);
	if (size < 0)
	{
		char *reply = "451 Read file error.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
	else
	{
		char *reply = "226 RETR successfully.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
}

void ftp_stor(Command *cmd, int connfd, int datafd, char *dir)
{
	char filename[200];
	strcpy(filename, dir);
	strcat(filename, "/");
	strcat(filename, cmd->arg);
	printf("filename:%s\n", filename);
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	char buffer[100];
	int size;
	while ((size = m_read_data(datafd, buffer, 100)) > 0)
	{
		if (write(fd, buffer, size) < 0)
		{
			char *reply = "451 Read file error.\r\n";
			m_write(connfd, reply, strlen(reply));
			close(fd);
			return;
		}
		if (size < 100)
		{
			break;
		}
	}
	close(fd);
	if (size < 0)
	{
		char *reply = "426 Connection closed.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
	char *reply = "226 STOR successfully.\r\n";
	m_write(connfd, reply, strlen(reply));
}

void ftp_quit(Command *cmd, int connfd, int *state)
{
	char *reply = "221 QUIT successfully.\r\n";
	*state = 1;
	m_write(connfd, reply, strlen(reply));
}

void ftp_syst(Command *cmd, int connfd)
{
	char *reply = "215 UNIX Type: L8\r\n";
	m_write(connfd, reply, strlen(reply));
}

void ftp_type(Command *cmd, int connfd)
{
	if (strcmp(cmd->arg, "I") == 0)
	{
		char *reply = "200 Type set to I.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
	else
	{
		char *reply = "504 Type set failed.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
}

void ftp_port(Command *cmd, int connfd, char *data_ip, int *data_port)
{
	int h1, h2, h3, h4, p1, p2;
	sscanf(cmd->arg, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
	sprintf(data_ip, "%d.%d.%d.%d", h1, h2, h3, h4);
	*data_port = p1 * 256 + p2;
	char *reply = "200 PORT successfully.\r\n";
	m_write(connfd, reply, strlen(reply));
}

void ftp_pasv(Command *cmd, int connfd, int *data_socket)
{
	srand((unsigned)time(NULL));
	char *ip = "127.0.0.1";
	int port;
	while (1)
	{
		port = rand() % (65536 - 20000) + 20000;
		if ((*data_socket = create_socket(ip, port)) != -1)
		{
			break;
		}
	}
	char reply[64];
	int h1, h2, h3, h4, p1, p2;
	sscanf(ip, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
	p2 = port % 256;
	p1 = port / 256;
	sprintf(reply, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", h1, h2, h3, h4, p1, p2);
	m_write(connfd, reply, strlen(reply));
}

void ftp_mkd(Command *cmd, int connfd, char *dir) {}

void ftp_cwd(Command *cmd, int connfd, char *dir) {}

void ftp_pwd(Command *cmd, int connfd, char *dir) {}

void ftp_list(Command *cmd, int connfd, int datafd, char *dir)
{
	char filename[200];
	strcpy(filename, dir);
	strcat(filename, "/");
	strcat(filename, cmd->arg);
	char name[200];
	sprintf(name, "ls -al %s", filename);
	FILE *f = popen(name, "r");
	char buffer[100];
	int size;
	while ((size = fread(buffer, 1, 100, f)) > 0)
	{
		printf("size=%d", size);
		if (m_write(datafd, buffer, size) < 0)
		{
			printf("1");
			char *reply = "426 Connection closed.\r\n";
			m_write(connfd, reply, strlen(reply));
			pclose(f);
			return;
		}
	}
	pclose(f);
	if (size > 100)
	{
		char *reply = "451 Read file error.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
	else
	{
		char *reply = "226 LIST successfully.\r\n";
		m_write(connfd, reply, strlen(reply));
	}
}

void ftp_rmd(Command *cmd, int connfd, char *dir) {}

void ftp_rnfr(Command *cmd, int connfd, char *dir) {}

void ftp_rnto(Command *cmd, int connfd, char *dir) {}
