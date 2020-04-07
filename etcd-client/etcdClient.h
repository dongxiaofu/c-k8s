//
// Created by chugang on 2020/4/7.
//
#include "http/http.h"

typedef struct Pod{
    char *id;
    char *config;
}Pod;

void *makePodKey(char *podId, char *podKey);

void createPod(Pod pod, char *body);

void getPod(char *params, char *body);




