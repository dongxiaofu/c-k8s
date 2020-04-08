//
// Created by chugang on 2020/4/7.
//

#ifndef C_K8S_HTTP_H
#define C_K8S_HTTP_H


#include <strings.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>
#include <regex.h>

int tcpConnect(const char *host);

#define BUFSIZE 1024
#define SERVER_PORT 2379
#define SERVER_PORT_STR "2379"
#define ETCD_HOST "127.0.0.1"

typedef struct EtcdParams{
    char *key;
    char *params;
}EtcdParams;

void put(EtcdParams *params, char *body, char *host);

void get(char *params, char *body, char *host);

void parseHttp(char *data, char *body);

int getContentLength(char *line);




#endif //C_K8S_HTTP_H
