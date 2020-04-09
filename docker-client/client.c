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
//    printf("7777");
//    puts("777");
    int sockfd = unixConn();

    fd_set fdSet;
    struct timeval tv;
    int dataLen = BUF_SIZE * 4;
    char str1[dataLen];
    memset(str1, 0, dataLen);
    strcat(str1, "GET /v1.39/containers/json?filters={\"status\":[\"running\"]}");
    strcat(str1, " HTTP/1.1\n");
    strcat(str1, "Host: 127.0.0.1\n");
    // todo 请求头结束之后，只有一个回车换行符\r\n。无论有无实体数据，这个分割符不能少。
    strcat(str1, "\r\n");

    write(sockfd, str1, strlen(str1));

    FD_ZERO(&fdSet);

    FD_SET(sockfd, &fdSet);

    int const bodyDataLength = sizeof(char) * BUF_SIZE * 70;
//    printf("7777");
//    printf("==============bodyDataLength==========%d\n", 3);
    char *bodyData = (char *) malloc(sizeof(char) * BUF_SIZE * 70);
//    memset(bodyData, 0, sizeof(char) * BUF_SIZE * 70);
//    char *bodyData = NULL;
    int bodyLength = -1;
    int total = 0;
    while (1) {
        // todo 不停顿，异常；停顿，如何提升吞吐量？
        sleep(1);
        tv.tv_sec = 1;
        tv.tv_usec = 1;
        int h = 0;
        h = select(sockfd + 1, &fdSet, NULL, NULL, &tv);
        if (h == 0) {
            printf("================h=0\n");
            close(sockfd);
            break;
        }
        if (h < 0) {
            close(sockfd);
            perror("select");
            return NULL;
        }
        if (h > 0) {
            int dataSize = sizeof(char) * BUF_SIZE;
            char *line = (char *) malloc(dataSize);
            printf("=================start to read\n");
            int len = read(sockfd, line, dataSize);
            total += len;
            printf("==========total = %d\n", total);
            int lineLength = strlen(line);
            printf("==========len = %d\tdatSize = %d\tlength = %d\n", len, dataSize, lineLength);
            printf("=============line\n%s\n", line);
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
            char *body = NULL;
            if (bodyLength == -1) {
                body = parseHttp(tmp);
            } else {
//                strcat(body, tmp);
                body = tmp;
            }
            strcat(bodyData, body);

            if (bodyLength == -1) {
                printf("==================counter===========\n");
//                char *tmp2 = (char *) malloc(strlen(line));
//                memset(tmp2, 0, strlen(line));
//                strcat(tmp2, line);
                // todo 当getContentLength执行结束，tmp2会被释放吗？
                bodyLength = getContentLength(line);
//                bodyLength = 43011;
            }
//            free(line);
//            line = NULL;
            int bodyDataLength2 = strlen(bodyData);
            printf("\n444444444=====%d=====%d====\n", bodyLength, bodyDataLength2);
            // 当select值为0时断开，这里是否没有必要？
            if (bodyLength == bodyDataLength2) {
                printf("\n=====%d=====%d====\n", bodyLength, bodyDataLength2);
//                close(sockfd);
//                free(line);
//                free(tmp);
//                break;
            }

//            free(line);
//            free(tmp);
        }
    }
    printf("\n=================over\n");
    return bodyData;
};


char *parseHttp(char *data) {
    char *nbody = (char *) malloc(sizeof(char) * BUF_SIZE);
    char *r = nbody;
//    memset(nbody, 0, sizeof(char) * BUF_SIZE);
    // 脏数据，不知道是什么
//    char line[BUF_SIZE];

//    memset(line, 0, BUF_SIZE);
    int j = 0;
    int k = 0;
//    memset(body, 0, BUF_SIZE);
    for (int i = 0; i < strlen(data); i++) {
//        char c;
//        c = data[i];
        if (data[i] != 10 && data[i] != 13) {
            continue;
//            line[j++] = c;
        } else {
            if (data[i++] == 13) {
                if (data[i++] == 10) {
                    if (data[i++] == 13) {
                        if (data[i++] == 10) {
                            for (; i < strlen(data); i++) {
                                r[k++] = data[i];
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
//            memset(line, 0, BUF_SIZE);
//            j = 0;
        }
    }

    return nbody;
}

int getContentLength(const char *line) {
//    return 3684;
//    char *nline = line;
//    memset(nline, 0, strlen(line));
//    memcpy(nline, line, strlen(line));
    int length = 0;
    regmatch_t *pmatch = (regmatch_t *) malloc(sizeof(regmatch_t));
    regex_t *regex = (regex_t *) malloc(sizeof(regex_t));
    char *pattern = "Content-Length";
//    char *pattern = "Content-Length";
    regcomp(regex, pattern, REG_EXTENDED);
    int offset = 0;
    while (offset < strlen(line)) {
        int status = regexec(regex, line + offset, 1, pmatch, 0);
        /* 匹配正则表达式，注意regexec()函数一次只能匹配一个，不能连续匹配，网上很多示例并没有说明这一点 */
        if (status == REG_NOMATCH) {
            //            printf("No Match\n");
        } else if (pmatch->rm_so != -1) {
            char *rest = NULL;
            char c = ':';
            rest = strchr(line + offset + pmatch->rm_eo, c);
            rest++;
            length = atoi(rest);
//            free(rest);     // pointer being freed was not allocated
            return length;
        } else {
            perror("match error");
        }
        offset += pmatch->rm_eo;
    }
    regfree(regex);        //释放正则表达式
    free(pattern);
    free(pmatch);

    return length;
}
