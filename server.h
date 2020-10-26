#ifndef SERVER_H
#define SERVER_H


typedef struct Command
{
  char command[5];
  char arg[1024];
} Command;


int m_write(int socketfd, char* msg, int len);
int create_socket(char *ip, int port);


#endif