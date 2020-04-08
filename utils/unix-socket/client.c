//
// Created by chugang on 2020/4/8.
//
#include "client.h"

int client(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s msg\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    char buf[BUF_SIZE + 1];
    int nbuf;

    nbuf = strlen(argv[1]);
    send(sockfd, argv[1], nbuf, 0);
    nbuf = recv(sockfd, buf, BUF_SIZE, 0);
    buf[nbuf] = 0;
    printf("echo msg: \"%s\"\n", buf);

    close(sockfd);
    return 0;
}

