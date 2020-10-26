#ifndef HANDLE_H
#define HANDLE_H
#include "server.h"

void ftp_user(Command *cmd, int connfd, int *state);
void ftp_pass(Command *cmd, int connfd, int *state);
void ftp_retr(Command *cmd, int connfd);
void ftp_stor(Command *cmd, int connfd);
void ftp_quit(Command *cmd, int connfd);
void ftp_syst(Command *cmd, int connfd);
void ftp_type(Command *cmd, int connfd);
void ftp_port(Command *cmd, int connfd);
void ftp_pasv(Command *cmd, int connfd);
void ftp_mkd(Command *cmd, int connfd);
void ftp_cwd(Command *cmd, int connfd);
void ftp_pwd(Command *cmd, int connfd);
void ftp_list(Command *cmd, int connfd);
void ftp_rmd(Command *cmd, int connfd);
void ftp_rnfr(Command *cmd, int connfd);
void ftp_rnto(Command *cmd, int connfd);

#endif