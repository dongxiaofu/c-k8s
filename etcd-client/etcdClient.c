//
// Created by chugang on 2020/4/7.
//
#include "http/http.h"

typedef struct Pod{
    char *id;
    char *config;
}Pod;

void makePodKey(char *podId, char *podKey);

void createPod(Pod pod, char *body);

void getPod(char *params, char *body);

void makePodKey(char *podId, char *podKey) {
    memset(podKey, 0, BUFSIZE);
    strcat(podKey, "/registry/pods/");
    strcat(podKey, podId);
}

void createPod(Pod pod, char *body) {
    char *podKey = (char *)malloc(sizeof(char) * BUFSIZE);
    makePodKey(pod.id, podKey);
    char *podConfig = pod.config;
    EtcdParams *etcdParams = (EtcdParams *)malloc(sizeof(EtcdParams));
    etcdParams->key = podKey;
    etcdParams->params = podConfig;
    put(etcdParams, body, ETCD_HOST);
}

void getPod(char *params, char *body) {

}



