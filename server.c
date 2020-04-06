//
// Created by chugang on 2020/4/6.
//

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <stdlib.h>

#define PORT 2369

int main() {
    int serverSocket;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int client;
    int addr_len = sizeof(client_addr);

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 2;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        return 3;
    }

    while (1) {
        client = accept(serverSocket, (struct sockaddr *) &client_addr, (socklen_t *) &addr_len);
        if (client < 0) {
            perror("accept");
            continue;
        }

        printf("client : %d\n", client);

        char *string = "hi";
        write(client, string, strlen(string));

//        char *str = (char *) malloc(sizeof(char));
//        read(client, str, strlen(str));
//        printf("str = %s\n", str);
//        write(client, str, strlen(str));

        while (1){
            printf("接收信息\n");
            char str2[200];
            printf("str2 len = %lu\n", strlen(str2));
            int len = read(client, str2, 200);
            if(len <= 0){
                printf("无数据\t");
                sleep(5);
                continue;
            }
            printf("str2 = %s\n", str2);
            sleep(5);
            printf("sleep over\n");
        }
    }

    return 0;
}
