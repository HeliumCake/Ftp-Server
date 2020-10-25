#include "handle.h"
#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>


int m_read(int socketfd, char* msg, int len) {
    int total = 0;
    int n;
    while((n = read(socketfd, msg+total, len-total)) > 0) {
        total += n;
        printf("r:%d %d ", total, n);
    }
    if (n < 0) {
        return n;
    }
    *(msg+total) = '\0';
    printf("r:%s", msg);
    return total;
}


int m_write(int socketfd, char* msg, int len) {
    int total = 0;
    int n;
    while((n = write(socketfd, msg+total, len-total)) > 0) {
        total += n;
        printf("w:%d %d ",total, n);
    }
    if (n < 0) {
        return n;
    }
    printf("w:%s", msg);
    return total;
}


void parse_command(char *str, Command *cmd) {
    int p;
	for (p = 0; p < 5; p++) {
		if (str[p] != ' ' && str[p] != '\0') {
			cmd->command[p] = str[p];
		}
		else {
			break;
	    }
	}
    cmd->command[p] = '\0';
	p++;
	int q = p;
	for (; str[p] != '\0'; p++) {
		if (str[p] != '\r'  || str[p+1] != '\n') {
			cmd->arg[p-q] = str[p];
		}
		else {
            break;
		}
	}
    cmd->arg[p-q] = '\0';
}


void* communication(void *arg) {
    int connfd = *(int*)arg;
    char buffer[1024];
    memset(buffer, 0, 1024);
    int username_ok = 0;
    int logged_in = 0;
    Command *cmd = (Command*)malloc(sizeof(Command));
    
    char *ready = "220 FTP server ready.\r\n";
    m_write(connfd, ready, strlen(ready));
    while (m_read(connfd, buffer, 1024) > 0) {
        printf("command:%s\n", buffer);
        parse_command(buffer, cmd);
        if (strcmp(cmd->command, "USER") == 0) {
            ftp_user(cmd, connfd);
        }
        memset(buffer, 0, 1024);
        memset(cmd, 0, sizeof(Command));
    }
    close(connfd);
    return NULL;
}


int main(int argc, char **argv) {
	int listenfd, connfd;		//监听socket和连接socket不一样，后者用于数据传输
	struct sockaddr_in addr;

	//创建socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//设置本机的ip和port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = 6789;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//监听"0.0.0.0"

	//将本机的ip和port与socket绑定
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//开始监听socket
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//持续监听连接请求
	while (1) {
		//等待client的连接 -- 阻塞函数
		if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			continue;
		}

        pthread_t pid;
        pthread_create(&pid, NULL, communication, &connfd);
	}

	close(listenfd);
}

