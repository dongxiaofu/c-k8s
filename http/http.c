//
// Created by chugang on 2020/4/7.
//
#include "http.h"

void put(EtcdParams *etcdParams, char *bodyData, char *host) {
    char *params = etcdParams->params;
    char *key = etcdParams->key;
    int clientSocket = tcpConnect(host);
    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUFSIZE * 4;
    char str1[dataLen], str2[dataLen];
    memset(str1, 0, dataLen);
    strcat(str1, "PUT /v2/keys");
    strcat(str1, etcdParams->key);
    strcat(str1, " HTTP/1.1\n");
    strcat(str1, "Connection:closed\n");
    strcat(str1, "Host:");
    strcat(str1, host);
    strcat(str1, ":");
    strcat(str1, SERVER_PORT_STR);
    strcat(str1, "\n");
    strcat(str1, "Content-Type:application/x-www-form-urlencoded\n");
    strcat(str1, "Content-Length:");
    memset(str2, 0, dataLen);
    strcat(str2, params);
    char *contentLength = (char *) malloc(128);
    sprintf(contentLength, "%lu", strlen(str2) + strlen("value="));
    strcat(str1, contentLength);
    strcat(str1, "\n");
    // todo 请求头结束之后，只有一个回车换行符\r\n
    strcat(str1, "\r\n");
    strcat(str1, "value=");
    strcat(str1, params);

    write(clientSocket, str1, strlen(str1));

    FD_ZERO(&fdSet);
    FD_SET(clientSocket, &fdSet);

    memset(bodyData, 0, BUFSIZE);
    int bodyLength = -1;
    while (1) {
        sleep(1);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int h = 0;
        h = select(clientSocket + 1, &fdSet, NULL, NULL, &tv);
        if (h == 0) {
            continue;
        }
        if (h < 0) {
            close(clientSocket);
            perror("select");
            return;
        }
        if (h > 0) {
            int dataSize = sizeof(char) * BUFSIZE;
            char *line = (char *) malloc(dataSize);
            int len = read(clientSocket, line, dataSize);
            if (len == -1) {
                printf("read over\n");
                break;
            }
            // todo 重复代码；第一次写出来的代码，第二次写，竟然无能力快速写对。
            // 思路：第一次收到的数据保护HTTP请求头，将此去掉，才能获得实体数据；
            // 第二次收到的数据全部是实体数据，不必处理，直接拼接到第一次解析出来的实体数据后面。
            // 第一次收到数据，从HTTP请求头中获取实体数据长度L。当接收到的实体数据长度和L相等时，断开连接。
            // 注意C语法，不能使用line，若使用，会出现死循环。原因不明，只能留心。
            char *tmp = (char *) malloc(sizeof(line));
            memset(tmp, 0, sizeof(line));
            strcat(tmp, line);
            char *body = (char *) malloc(sizeof(char) * BUFSIZE);
            if (bodyLength == -1) {
                parseHttp(tmp, body);
            } else {
                strcat(body, line);
            }

            strcat(bodyData, body);
            if (bodyLength == -1) {
                bodyLength = getContentLength(line);
            }
            if (bodyLength == strlen(bodyData)) {
                close(clientSocket);
                break;
            }
        }
    }
}

void get(char *params, char *bodyData, char *host) {
    int clientSocket = tcpConnect(host);
    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUFSIZE * 4;
    char str1[dataLen];
    memset(str1, 0, dataLen);
    strcat(str1, "GET /v2/keys");
    strcat(str1, params);
    strcat(str1, " HTTP/1.1\n");
    strcat(str1, "Host:");
    strcat(str1, host);
    strcat(str1, ":");
    strcat(str1, SERVER_PORT_STR);
    strcat(str1, "\n");
    strcat(str1, "Connection: closed\n");
    strcat(str1, "\r\n");

    write(clientSocket, str1, strlen(str1));

    FD_ZERO(&fdSet);
    FD_SET(clientSocket, &fdSet);
    int bodyDataLength = sizeof(char) * BUFSIZE * 4;
    memset(bodyData, 0, bodyDataLength);
    int contentLength = -1;
    while (1) {
        sleep(2);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int h = select(clientSocket + 1, &fdSet, NULL, NULL, &tv);
        if (h == 0) {
            continue;
        }
        if (h < 0) {
            perror("select");
            return;
        }
        if (h > 0) {
            char *str = (char *) malloc(sizeof(char) * BUFSIZE);
            memset(str, 0, BUFSIZE);
            int n = read(clientSocket, str, BUFSIZE);
            if (n == -1) {
                break;
            }
            char *body = (char *) malloc(sizeof(char) * BUFSIZE);
            memset(body, 0, dataLen);
            char *tmp = (char *) malloc(sizeof(str));
            memset(tmp, 0, sizeof(str));
            strcat(tmp, str);
            if (contentLength == -1) {
                // todo 第一个参数使用str时，str存储的数据无穷无尽，有超过百万个字符。
                // todo 原因未知，使用tmp代替str后，问题消失。下次我可能还会犯这个错误。
                parseHttp(tmp, body);
            } else {
                strcat(body, tmp);
            }
            strcat(bodyData, body);
            if (contentLength == -1) {
                // todo 神奇，此处若使用tmp，getContentLength内部循环就是死循环
                // todo 是因为，上面parseHttp改变了tmp吗？
                // todo 暂不理会这类问题，遇到了奇怪问题，就朝着方面想，猜测修改
                contentLength = getContentLength(str);
            }
            free(tmp);
//            free(str);
            int bodyDataLength = strlen(bodyData);
            if (bodyDataLength == contentLength) {
                close(clientSocket);
                break;
            }
        }
    }
}

void parseHttp(char *data, char *body) {
    char line[BUFSIZE];
    memset(line, 0, BUFSIZE);
    int j = 0;
    int k = 0;
    memset(body, 0, BUFSIZE);
    for (int i = 0; i < strlen(data); i++) {
        char c;
        c = data[i];
        if (data[i] != 10 && data[i] != 13) {
            line[j++] = c;
        } else {
            if (data[i++] == 13) {
                if (data[i++] == 10) {
                    if (data[i++] == 13) {
                        if (data[i++] == 10) {
                            for (; i < strlen(data); i++) {
                                body[k++] = data[i];
                            }
                            break;
                        } else {
                            i--;
                        }
                    } else {
                        i--;
                    }
                } else {
                    i--;
                }
            } else {
                i--;
            }
            memset(line, 0, BUFSIZE);
            j = 0;
        }
    }
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

int getContentLength(char *line) {
    int length = 0;
    regmatch_t pmatch;
    regex_t regex;
    const char *pattern = "Content-Length";
    regcomp(&regex, pattern, REG_EXTENDED);
    int offset = 0;
    while (offset < strlen(line)) {
        int status = regexec(&regex, line + offset, 1, &pmatch, 0);
        /* 匹配正则表达式，注意regexec()函数一次只能匹配一个，不能连续匹配，网上很多示例并没有说明这一点 */
        if (status == REG_NOMATCH) {
            //            printf("No Match\n");
        } else if (pmatch.rm_so != -1) {
            char *rest;
            char c = ':';
            rest = strchr(line + offset + pmatch.rm_eo, c);
            rest++;
            length = atoi(rest);
            return length;
        }
        offset += pmatch.rm_eo;
    }
    regfree(&regex);        //释放正则表达式
    return length;
}

