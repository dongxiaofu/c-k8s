//
// Created by chugang on 2020/4/7.
//

#include "etcd-client/etcdClient.h"

int main() {
    Pod pod;
    pod.id = "nginx2";
    pod.config = "{\n"
                 "  \"kind\": \"Pod\",\n"
                 "  \"apiVersion\": \"v1beta1\",\n"
                 "  \"id\": \"php\",\n"
                 "  \"desiredState\": {\n"
                 "    \"manifest\": {\n"
                 "      \"version\": \"v1beta1\",\n"
                 "      \"id\": \"php\",\n"
                 "      \"containers\": [{\n"
                 "        \"name\": \"nginx\",\n"
                 "        \"image\": \"nginx\",\n"
                 "        \"ports\": [{\n"
                 "          \"containerPort\": 80,\n"
                 "          \"hostPort\": 8082\n"
                 "        }],\n"
                 "        \"livenessProbe\": {\n"
                 "          \"enabled\": true,\n"
                 "          \"type\": \"http\",\n"
                 "          \"initialDelaySeconds\": 30,\n"
                 "          \"httpGet\": {\n"
                 "            \"path\": \"/index.html\",\n"
                 "            \"port\": \"8082\"\n"
                 "          }\n"
                 "        }\n"
                 "      }]\n"
                 "    }\n"
                 "  },\n"
                 "  \"labels\": {\n"
                 "    \"name\": \"foo\"\n"
                 "  }\n"
                 "}\n"
                 "";
//    pod.config = "hello23-nginx";
    char *body = (char *)malloc(sizeof(char) * BUFSIZE*4);
    createPod(pod, body);
    printf("body:\n%s", body);
}


