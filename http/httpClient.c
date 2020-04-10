//
// Created by chugang on 2020/4/10.
//

#include "httpClient.h"
#include "http.h"

Response doRequest(char *protocol, char *method, char *path, char *data) {
    int clientSocket = -1;
    char statusCode[16];
    int total = sizeof(char) * BUFSIZ * 2;
    char *body = (char *) malloc(total);
    int m = 0;
    int contentLength = 0;
    int startBody = 0;  // 接收数据是否已经到达主体部分
//    char *host = "49.235.149.104";
    char *host = "127.0.0.1";
    if (strcasecmp(protocol, "http") == 0) {
        clientSocket = tcpConnect(host);
    } else if (strcasecmp(protocol, "unix") == 0) {
        clientSocket = unixConn();
    }

    char buf[1024];
    sprintf(buf, "%s %s HTTP/1.1\r\n", method, path);
    write(clientSocket, buf, strlen(buf));
    sprintf(buf, "Host: %s\r\n", host);
    write(clientSocket, buf, strlen(buf));
    sprintf(buf, "\r\n");
    write(clientSocket, buf, strlen(buf));

    fd_set fdSet;
    struct timeval tv;
    FD_ZERO(&fdSet);


    while (1) {
        FD_SET(clientSocket, &fdSet);
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
            break;
        }

        if (h > 0) {
            int s = BUFSIZ * 2;
            char buf2[s];
            memset(buf2, 0, s);
            if (contentLength == 0) {
                int charNum = readLine(clientSocket, buf, sizeof(buf));
                char protocol[16];

                if (charNum) {
                    int j = 0;
                    for (int i = 0; buf[i] != '\n'; i++) {
                        if (isspace(buf[i]) == 0) {
                            protocol[j++] = buf[i];
                        } else {
                            protocol[j] = '\0';
                            i++;
                            if (strcasecmp(protocol, "HTTP/1.1") == 0) {
                                int k = 0;
                                while (buf[i] != '\n' && isspace(buf[i]) == 0) {
                                    statusCode[k++] = buf[i];
                                    i++;
                                }
                                statusCode[k] = '\0';
                                break;
                            } else {
                                protocol[0] = '\0';
                                break;
                            }
                        }
                    }
                }
                // Content-Length: 6699
                while (1) {
                    int charNum = readLine(clientSocket, buf, sizeof(buf));
                    char headMeta[16];
                    char length[16];
                    if (charNum) {
                        int i = 0;
                        int j = 0;
                        while (buf[i] != '\n' && i < BUFSIZ - 1) {
                            if (isspace(buf[i]) == 0) {
                                headMeta[j++] = buf[i++];
                            } else {
                                headMeta[j] = '\0';
                                i++;
                                if (strcasecmp(headMeta, "Content-Length:") == 0) {
                                    int k = 0;
                                    while (buf[i] != '\n' && isspace(buf[i]) == 0) {
                                        length[k++] = buf[i++];
                                    }
                                    contentLength = atoi(length);
                                }
                                break;
                            }
                        }
                        if (contentLength > 0) {
                            break;
                        }
                    }
                }
            }
            buf[0] = '\0';

            int n = read(clientSocket, buf2, sizeof(buf2));

            if (startBody == 0) {
                for (int i = 0; i < n; i++) {
                    if (buf2[i] == '\r') {
                        if (i + 1 < n && buf2[i + 1] == '\n') {
                            if (i + 2 < n && buf2[i + 2] == '\r') {
                                if (i + 3 < n && buf2[i + 3] == '\n') {
                                    startBody = 1;
                                    i = i + 3;
                                    continue;
                                }
                            }
                        }
                    }
                    if (startBody == 1) {
                        body[m++] = buf2[i];
                    }
                }
            } else {
                if (m == total - 1) {
                    total = total * 2;
                    body = (char *) realloc(body, total);
                }
                for (int i = 0; i < n; i++) {
                    body[m++] = buf2[i];
                }
            }

            if (m  == contentLength) {
                break;
            }
        }
    }

    body[m] = '\0';

    Response response = {atoi(statusCode), body};

    return response;
}

int readLine(int socket, char *buf, int size) {
    int i = 0;
    char c = '\0';
    while (i < size - 1 && c != '\n') {
        int n = read(socket, &c, 1);
        if (n > 0) {
            if (c == '\r') {
                c = '\n';
            }
        } else {
            c = '\n';
        }
        buf[i] = c;
        i++;
    }

    buf[i] = '\0';

    return i;
}

