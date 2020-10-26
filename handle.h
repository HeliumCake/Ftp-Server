#ifndef HANDLE_H
#define HANDLE_H
#include "server.h"

void ftp_user(Command *cmd, int connfd, int *state);
void ftp_pass(Command *cmd, int connfd, int *state);
void ftp_retr(Command *cmd, int connfd, int datafd, char *dir);
void ftp_stor(Command *cmd, int connfd, int datafd, char *dir);
void ftp_quit(Command *cmd, int connfd, int *state);
void ftp_syst(Command *cmd, int connfd);
void ftp_type(Command *cmd, int connfd);
void ftp_port(Command *cmd, int connfd, char *data_ip, int *data_port);
void ftp_pasv(Command *cmd, int connfd, int *data_socket);
void ftp_mkd(Command *cmd, int connfd, char *dir);
void ftp_cwd(Command *cmd, int connfd, char *dir);
void ftp_pwd(Command *cmd, int connfd, char *dir);
void ftp_list(Command *cmd, int connfd, int datafd, char *dir);
void ftp_rmd(Command *cmd, int connfd, char *dir);
void ftp_rnfr(Command *cmd, int connfd, char *dir, char *oldname);
void ftp_rnto(Command *cmd, int connfd, char *dir, char *oldname);

#endif