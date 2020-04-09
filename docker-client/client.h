//
// Created by chugang on 2020/4/8.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/wait.h>

#define SOCK_PATH "/var/run/docker.sock"
#define BUF_SIZE 1024

int unixConn();

char *getContainerList(char *);

char *getContainer();

char *createContainer();

char *parseHttp(char *data);

int getContentLength(const char *line);
