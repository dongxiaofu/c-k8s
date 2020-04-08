//
// Created by chugang on 2020/4/8.
//
#include <_regex.h>
#include <regex.h>
#include "client.h"

int unixConn() {
    int sockfd;
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SOCK_PATH);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

char *createContainer() {

    int sockfd = unixConn();

    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUF_SIZE * 4;
    char str1[dataLen], str2[dataLen];
    memset(str1, 0, dataLen);
    strcat(str1, "GET /v1.39/containers/json?filters={\"status\":[\"exited\"]}");
    strcat(str1, " HTTP/1.1\n");
    strcat(str1, "Host: 127.0.0.1\n");
    // todo 请求头结束之后，只有一个回车换行符\r\n。无论有无实体数据，这个分割符不能少。
    strcat(str1, "\r\n");
    write(sockfd, str1, strlen(str1));

    FD_ZERO(&fdSet);
    FD_SET(sockfd, &fdSet);

    int bodyDataLength = sizeof(char) * BUF_SIZE * 4;
    char *bodyData = (char *) malloc(bodyDataLength);
    memset(bodyData, 0, bodyDataLength);
    int bodyLength = -1;
    while (1) {
        // todo 不停顿，异常；停顿，如何提升吞吐量？
        sleep(1);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int h = 0;
        h = select(sockfd + 1, &fdSet, NULL, NULL, &tv);
        if (h == 0) {
            continue;
        }
        if (h < 0) {
            close(sockfd);
            perror("select");
            return NULL;
        }
        if (h > 0) {
            int dataSize = sizeof(char) * BUF_SIZE;
            char *line = (char *) malloc(dataSize);
            int len = read(sockfd, line, dataSize);
            printf("=============line\n%s", line);
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
//            continue;
            char *body = (char *) malloc(sizeof(char) * BUF_SIZE);
            if (bodyLength == -1) {
                parseHttp(tmp, body);
            } else {
                strcat(body, tmp);
            }

            strcat(bodyData, body);

            if (bodyLength == -1) {
                bodyLength = getContentLength(line);
            }
//            free(line);
            if (bodyLength == strlen(bodyData)) {
                printf("=====%d=====%lu====\n", bodyLength, strlen(bodyData));
                close(sockfd);
                break;
            }
        }
    }

    return bodyData;
};


void parseHttp(char *data, char *body) {
    char line[BUF_SIZE];
    memset(line, 0, BUF_SIZE);
    int j = 0;
    int k = 0;
    memset(body, 0, BUF_SIZE);
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
            memset(line, 0, BUF_SIZE);
            j = 0;
        }
    }
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
