#include "handle.h"
#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

int m_read(int socketfd, char *msg, int len)
{
    int total = 0;
    int n;
    while ((n = read(socketfd, msg + total, len - total)) > 0)
    {
        total += n;
        if (msg[total - 2] == '\r' && msg[total - 1] == '\n')
        {
            break;
        }
    }
    if (n < 0)
    {
        return n;
    }
    msg[total] = '\0';
    return total;
}

int m_read_data(int socketfd, char *msg, int len)
{
    int total = 0;
    int n;
    while ((n = read(socketfd, msg + total, len - total)) > 0)
    {
        total += n;
    }
    if (n < 0)
    {
        return n;
    }
    return total;
}

int m_write(int socketfd, char *msg, int len)
{
    int total = 0;
    int n;
    while ((n = write(socketfd, msg + total, len - total)) > 0)
    {
        total += n;
    }
    if (n < 0)
    {
        return n;
    }
    return total;
}

void parse_command(char *str, Command *cmd)
{
    int p;
    for (p = 0; p < 5; p++)
    {
        if (str[p] != ' ' && str[p] != '\0' && (str[p] != '\r' || str[p + 1] != '\n'))
        {
            cmd->command[p] = str[p];
        }
        else
        {
            break;
        }
    }
    cmd->command[p] = '\0';
    p++;
    int q = p;
    for (; str[p] != '\0'; p++)
    {
        if (str[p] != '\r' || str[p + 1] != '\n')
        {
            cmd->arg[p - q] = str[p];
        }
        else
        {
            break;
        }
    }
    cmd->arg[p - q] = '\0';
}

int create_connect(char *ip, int port)
{
    int datafd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = (in_port_t)htons((uint16_t)port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(datafd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        return -1;
    }

    return datafd;
}

int create_socket(char *ip, int port)
{
    int listenfd;
    struct sockaddr_in addr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = (in_port_t)htons((uint16_t)port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        return -1;
    }

    if (listen(listenfd, 10) == -1)
    {
        return -1;
    }

    return listenfd;
}

void *communication(void *arg)
{
    int connfd = *(int *)arg;
    char dir[200];
    strcpy(dir, root_dir);
    char buffer[1024];
    memset(buffer, 0, 1024);
    int username_ok = 0;
    int logged_in = 0;
    int rnfr_tag = 0;
    int quit_tag = 0;
    char data_ip[20];
    int data_port = -1;
    int data_socket = -1;
    char oldname[200];
    Command *cmd = (Command *)malloc(sizeof(Command));

    char *ready = "220 Anonymous FTP server ready.\r\n";
    m_write(connfd, ready, strlen(ready));
    while (m_read(connfd, buffer, 1024) > 0)
    {
        parse_command(buffer, cmd);
        if (strcmp(cmd->command, "USER") == 0)
        {
            rnfr_tag = 0;
            ftp_user(cmd, connfd, &username_ok);
        }
        else if (strcmp(cmd->command, "PASS") == 0)
        {
            rnfr_tag = 0;
            if (username_ok == 0)
            {
                char *non_user = "503 None USER.\r\n";
                m_write(connfd, non_user, strlen(non_user));
            }
            else
            {
                ftp_pass(cmd, connfd, &logged_in);
            }
            username_ok = 0;
        }
        else if (logged_in != 1)
        {
            username_ok = 0;
            char *not_logged_in = "530 Acessess denied.\r\n";
            m_write(connfd, not_logged_in, strlen(not_logged_in));
        }
        else
        {
            if (strcmp(cmd->command, "RETR") == 0)
            {
                rnfr_tag = 0;
                char *reply = "150 About to open data connection.\r\n";
                m_write(connfd, reply, strlen(reply));
                int data_connfd;
                if (data_socket != -1 && (data_connfd = accept(data_socket, NULL, NULL)) != -1)
                {
                    close(data_socket);
                    data_socket = -1;
                    ftp_retr(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else if (data_port != -1 && (data_connfd = create_connect(data_ip, data_port)) != -1)
                {
                    data_port = -1;
                    ftp_retr(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else
                {
                    char *reply = "425 Can't open data connection.\r\n";
                    m_write(connfd, reply, strlen(reply));
                }
            }
            else if (strcmp(cmd->command, "STOR") == 0)
            {
                rnfr_tag = 0;
                char *reply = "150 About to open data connection.\r\n";
                m_write(connfd, reply, strlen(reply));
                int data_connfd;
                if (data_socket != -1 && (data_connfd = accept(data_socket, NULL, NULL)) != -1)
                {
                    close(data_socket);
                    data_socket = -1;
                    ftp_stor(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else if (data_port != -1 && (data_connfd = create_connect(data_ip, data_port)) != -1)
                {
                    data_port = -1;
                    ftp_stor(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else
                {
                    char *reply = "425 Can't open data connection.\r\n";
                    m_write(connfd, reply, strlen(reply));
                }
            }
            else if (strcmp(cmd->command, "QUIT") == 0)
            {
                rnfr_tag = 0;
                ftp_quit(cmd, connfd, &quit_tag);
                if (quit_tag == 1)
                {
                    break;
                }
            }
            else if (strcmp(cmd->command, "ABOR") == 0)
            {
                rnfr_tag = 0;
                ftp_quit(cmd, connfd, &quit_tag);
                if (quit_tag == 1)
                {
                    break;
                }
            }
            else if (strcmp(cmd->command, "SYST") == 0)
            {
                rnfr_tag = 0;
                ftp_syst(cmd, connfd);
            }
            else if (strcmp(cmd->command, "TYPE") == 0)
            {
                rnfr_tag = 0;
                ftp_type(cmd, connfd);
            }
            else if (strcmp(cmd->command, "PORT") == 0)
            {
                rnfr_tag = 0;
                if (data_socket != -1)
                {
                    close(data_socket);
                    data_socket = -1;
                }
                if (data_port != -1)
                {
                    data_port = -1;
                }
                ftp_port(cmd, connfd, data_ip, &data_port);
            }
            else if (strcmp(cmd->command, "PASV") == 0)
            {
                rnfr_tag = 0;
                if (data_socket != -1)
                {
                    close(data_socket);
                    data_socket = -1;
                }
                if (data_port != -1)
                {
                    data_port = -1;
                }
                ftp_pasv(cmd, connfd, &data_socket);
            }
            else if (strcmp(cmd->command, "MKD") == 0)
            {
                rnfr_tag = 0;
                ftp_mkd(cmd, connfd, dir);
            }
            else if (strcmp(cmd->command, "CWD") == 0)
            {
                rnfr_tag = 0;
                ftp_cwd(cmd, connfd, dir);
            }
            else if (strcmp(cmd->command, "PWD") == 0)
            {
                rnfr_tag = 0;
                ftp_pwd(cmd, connfd, dir);
            }
            else if (strcmp(cmd->command, "LIST") == 0)
            {
                rnfr_tag = 0;
                char *reply = "150 About to open data connection.\r\n";
                m_write(connfd, reply, strlen(reply));
                int data_connfd;
                if (data_socket != -1 && (data_connfd = accept(data_socket, NULL, NULL)) != -1)
                {
                    close(data_socket);
                    data_socket = -1;
                    ftp_list(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else if (data_port != -1 && (data_connfd = create_connect(data_ip, data_port)) != -1)
                {
                    data_port = -1;
                    ftp_list(cmd, connfd, data_connfd, dir);
                    close(data_connfd);
                }
                else
                {
                    char *reply = "425 Can't open data connection.\r\n";
                    m_write(connfd, reply, strlen(reply));
                }
            }
            else if (strcmp(cmd->command, "RMD") == 0)
            {
                rnfr_tag = 0;
                ftp_rmd(cmd, connfd, dir);
            }
            else if (strcmp(cmd->command, "RNFR") == 0)
            {
                rnfr_tag = 1;
                ftp_rnfr(cmd, connfd, dir, oldname);
            }
            else if (strcmp(cmd->command, "RNTO") == 0)
            {
                if (rnfr_tag == 1)
                {
                    ftp_rnto(cmd, connfd, dir, oldname);
                }
                else
                {
                    char *non_rnfr = "503 None RNFR.\r\n";
                    m_write(connfd, non_rnfr, strlen(non_rnfr));
                }
                rnfr_tag = 0;
            }
        }

        memset(buffer, 0, 1024);
        memset(cmd, 0, sizeof(Command));
    }
    if (data_socket != -1)
    {
        close(data_socket);
    }
    close(connfd);
    return NULL;
}

int main(int argc, char **argv)
{
    char *ip = "0.0.0.0";
    int port = 21;
    strcpy(root_dir, "/tmp");
    if (argc == 3)
    {
        if (strcmp(argv[1], "-root") == 0)
        {
            strcpy(root_dir, argv[2]);
        }
        else if (strcmp(argv[1], "-port") == 0)
        {
            sscanf(argv[2], "%d", &port);
        }
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-root") == 0)
        {
            strcpy(root_dir, argv[2]);
        }
        else if (strcmp(argv[1], "-port") == 0)
        {
            sscanf(argv[2], "%d", &port);
        }
        if (strcmp(argv[3], "-root") == 0)
        {
            strcpy(root_dir, argv[4]);
        }
        else if (strcmp(argv[3], "-port") == 0)
        {
            sscanf(argv[4], "%d", &port);
        }
    }

    int listenfd, connfd;

    if ((listenfd = create_socket(ip, port)) == -1)
    {
        return 1;
    }

    while (1)
    {
        if ((connfd = accept(listenfd, NULL, NULL)) == -1)
        {
            continue;
        }

        pthread_t pid;
        pthread_create(&pid, NULL, communication, &connfd);
    }

    close(listenfd);
}
