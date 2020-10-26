#include "handle.h"
#include "server.h"
#include <string.h>

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
void ftp_retr(Command *cmd, int connfd) {}
void ftp_stor(Command *cmd, int connfd) {}
void ftp_quit(Command *cmd, int connfd, int *state)
{
	char *reply = "221 Quit successfully.\r\n";
	*state = 1;
	m_write(connfd, reply, strlen(reply));
}
void ftp_syst(Command *cmd, int connfd) {}
void ftp_type(Command *cmd, int connfd) {}
void ftp_port(Command *cmd, int connfd) {}
void ftp_pasv(Command *cmd, int connfd) {}
void ftp_mkd(Command *cmd, int connfd) {}
void ftp_cwd(Command *cmd, int connfd) {}
void ftp_pwd(Command *cmd, int connfd) {}
void ftp_list(Command *cmd, int connfd) {}
void ftp_rmd(Command *cmd, int connfd) {}
void ftp_rnfr(Command *cmd, int connfd) {}
void ftp_rnto(Command *cmd, int connfd) {}
