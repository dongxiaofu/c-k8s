//
// Created by chugang on 2020/4/10.
//
#include <ctype.h>

typedef struct {
    int statusCode;
    char *body;
} Response;

Response doRequest(char *protocol, char *method, char *path, char *data);

int readLine(int socket, char *buf, int size);






