#ifndef SERVER_H
#define SERVER_H


typedef struct Command
{
  char command[5];
  char arg[1024];
} Command;

char root_dir[100];

int m_read_data(int socketfd, char *msg, int len);
int m_write(int socketfd, char* msg, int len);
int create_socket(char *ip, int port);


#endif