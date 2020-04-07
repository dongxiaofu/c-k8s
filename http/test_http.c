//
// Created by chugang on 2020/4/7.
//

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "http/http.h"


int main() {
    char *body;
    char *params = (char *) malloc(sizeof(char) * BUFSIZE);
    memset(params, 0, strlen(params));
    strcat(params, "{\"key\": \"Zm9v\", \"value\": \"YmFy\"}");
    put(params, body, ETCD_HOST);
    printf("body:%s\n", body);
    putchar('\n');
    for (int i = 0; i < 20; i++) {
        putchar('*');
    }
    putchar('\n');
    char *params2 = (char *) malloc(sizeof(char) * BUFSIZE);
    memset(params2, 0, strlen(params2));
    strcat(params2, "{\"key\":\"Zm9v\"}");
    char *body2 = (char *) malloc(sizeof(char) * BUFSIZE);
    get(params2, body2, ETCD_HOST);
    printf("body2:%s\n", body2);


    return 0;
}
