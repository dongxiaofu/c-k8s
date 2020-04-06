//
// Created by chugang on 2020/4/6.
//

#include <strings.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <stdlib.h>
#include <regex.h>

int tcpConnect(const char *host);

void put(char *params, char *str);

void get(char *params, char *str);

void parseHttp(char *data, char *body);

#define BUFSIZE 1024
#define SERVER_PORT 2379
#define SERVER_PORT_STR "2379"
#define ETCD_HOST "127.0.0.1"

char *host = ETCD_HOST;

int main() {

    char *str;
    char *params = (char *) malloc(sizeof(char) * BUFSIZE);
    memset(params, 0, strlen(params));
    strcat(params, "{\"key\": \"Zm9v\", \"value\": \"YmFy\"}");
    put(params, str);
//    printf("output:%s\n", str);
    putchar('\n');
    for (int i = 0; i < 20; i++) {
        putchar('*');
    }
    putchar('\n');
    char *params2 = (char *) malloc(sizeof(char) * BUFSIZE);
    memset(params2, 0, strlen(params2));
    strcat(params2, "{\"key\":\"Zm9v\"}");
    char *str2 = (char *) malloc(sizeof(char) * BUFSIZE);
    get(params2, str2);


    return 0;
}

int tcpConnect(const char *host) {
    int clientSocket = -1;
    struct sockaddr_in serverAddr;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(host);

    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return 2;
    }

    printf("连接到etcd:%d\n", clientSocket);

    return clientSocket;
}

void put(char *params, char *str) {
    int clientSocket = tcpConnect(host);
    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUFSIZE * 4;
    char str1[dataLen], str2[dataLen];

    memset(str1, 0, dataLen);
    strcat(str1, "POST /v3/kv/put HTTP/1.1\n");
    strcat(str1, "Connection:closed\n");
    strcat(str1, "Host:");
    strcat(str1, ETCD_HOST);
    strcat(str1, ":");
    strcat(str1, SERVER_PORT_STR);
    strcat(str1, "\n");
    strcat(str1, "Content-Type:application/json;charset=utf-8");
    strcat(str1, "\n");
    strcat(str1, "Content-Length:");
    memset(str2, 0, dataLen);
    strcat(str2, params);
    char *contentLength = (char *) malloc(128);
    sprintf(contentLength, "%lu", strlen(str2));
    strcat(str1, contentLength);
    strcat(str1, "\n");
    // todo 请求头结束之后，只有一个回车换行符\r\n
    strcat(str1, "\r\n");
    strcat(str1, params);
    strcat(str1, "\n");

    write(clientSocket, str1, strlen(str1));

    FD_ZERO(&fdSet);
    FD_SET(clientSocket, &fdSet);

    while (1) {
        sleep(1);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int h = 0;
        h = select(clientSocket + 1, &fdSet, NULL, NULL, &tv);
        if (h < 0) {
            close(clientSocket);
            perror("select");
            return;
        }

        memset(str, 0, BUFSIZE);
        if (h > 0) {
            char *line = (char *) malloc(sizeof(char) * BUFSIZE);
            int len = read(clientSocket, line, BUFSIZE);
            if (len == -1) {
                printf("read over\n");
                break;
            }

            printf("received:%s\n", line);
            parseHttp(line, "");
            close(clientSocket);
            break;
        }
    }
}


void get(char *params, char *str) {
    int clientSocket = tcpConnect(host);
    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUFSIZE * 4;
    char str1[dataLen], str2[dataLen];

    memset(str1, 0, dataLen);
    strcat(str1, "POST /v3/kv/range HTTP/1.1\n");
    strcat(str1, "Host:");
    strcat(str1, ETCD_HOST);
    strcat(str1, ":");
    strcat(str1, SERVER_PORT_STR);
    strcat(str1, "\n");
    strcat(str1, "Connection:closed\n");
    strcat(str1, "Content-Type:application/json;charset=utf-8\n");

    char *contentLength = (char *) malloc(128);
    memset(str2, 0, dataLen);
    strcat(str2, params);
    sprintf(contentLength, "%lu", strlen(str2));
    strcat(str1, "Cotent-Length:");
    strcat(str1, contentLength);
    strcat(str1, "\n");
    strcat(str1, "\r\n");
    // 请求头结束，下面是请求主体
    strcat(str1, params);

    write(clientSocket, str1, strlen(str1));

    FD_ZERO(&fdSet);
    FD_SET(clientSocket, &fdSet);

    while (1) {
        sleep(2);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int h = select(clientSocket + 1, &fdSet, NULL, NULL, &tv);
        if (h < 0) {
            perror("select");
            return;
        }
        if (h > 0) {
            memset(str, 0, dataLen);
            int n = read(clientSocket, str, dataLen);
            if (n == -1) {
                break;
            }
            printf("%s\n", str);
        }
    }
}


void parseHttp(char *data, char *body) {
    char line[BUFSIZE];
    memset(line, 0, BUFSIZE);
    int j = 0;
    for (int i = 0; i < strlen(data); i++) {
//        printf("data[%d] = %c\n", i, data[i]);
//        continue;
        char c;
        c = data[i];
        if (data[i] != 10) {
//            printf("data[%d] = %c\n", i, data[i]);
//            strcat(line, &data[i]);
//            strcat(line, &c);
            line[j++] = c;
        } else {
//            printf("换行\n");
//            printf("%s\n", line);
            regmatch_t pmatch;
            regex_t regex;
            const char *pattern = "Content-Length";
            regcomp(&regex, pattern, REG_EXTENDED);
            int offset = 0;
            while (offset < strlen(line)) {
                int status = regexec(&regex, line + offset, 1, &pmatch, 0);
                /* 匹配正则表达式，注意regexec()函数一次只能匹配一个，不能连续匹配，网上很多示例并没有说明这一点 */
                if (status == REG_NOMATCH)
                    printf("No Match\n");
                else if (pmatch.rm_so != -1) {
                    printf("Match:\n");
                    printf("%s\n", line);
                    char *rest;
                    char c = ':';
                    rest = strchr(line, c);
                    *rest++;
                    printf("rest = %s\n", rest);
                    int length = atoi(rest);
                    printf("===============length = %d\n", length);
                }
                offset += pmatch.rm_eo;
            }
            regfree(&regex);        //释放正则表达式
            memset(line, 0, BUFSIZE);
            j = 0;
        }
    }
}