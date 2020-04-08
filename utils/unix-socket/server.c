//
// Created by chugang on 2020/4/8.
//
#include "server.h"

int server(void) {
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGTERM, handle_signal);

    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, SOCK_PATH);

    unlink(SOCK_PATH);
    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    chmod(SOCK_PATH, 00640);

    if (listen(listenfd, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int connfd, nbuf;
    char buf[BUF_SIZE + 1];
    for (;;) {
        if ((connfd = accept(listenfd, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }

        nbuf = recv(connfd, buf, BUF_SIZE, 0);
        buf[nbuf] = 0;
        printf("new msg: \"%s\"\n", buf);
        send(connfd, buf, nbuf, 0);

        close(connfd);
    }

    return 0;
}

void handle_signal(int signo) {
    if (signo == SIGINT) {
        fprintf(stderr, "received signal: SIGINT(%d)\n", signo);
    } else if (signo == SIGHUP) {
        fprintf(stderr, "received signal: SIGHUP(%d)\n", signo);
    } else if (signo == SIGTERM) {
        fprintf(stderr, "received signal: SIGTERM(%d)\n", signo);
    }

    close(listenfd);
    unlink(SOCK_PATH);
    exit(EXIT_SUCCESS);
}
