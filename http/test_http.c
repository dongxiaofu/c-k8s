//
// Created by chugang on 2020/4/7.
//

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "http/http.h"
#include "httpClient.h"

void test_parseHttp();

void test_doRequest();

int main() {
//    char *body;
//    char *params = (char *) malloc(sizeof(char) * BUFSIZE);
//    memset(params, 0, strlen(params));
////    {"key":"hello","value":"hello"}
//    strcat(params, "{\"taGVsbG8sd29ybGQ=\"}");
//    put(params, body, ETCD_HOST);
//    printf("body:%s\n", body);
//    putchar('\n');
//    for (int i = 0; i < 20; i++) {
//        putchar('*');
//    }
//    putchar('\n');
//    char *params2 = (char *) malloc(sizeof(char) * BUFSIZE);
//    memset(params2, 0, strlen(params2));
//    strcat(params2, "{\"key\":\"Zm9v\"}");
//    char *body2 = (char *) malloc(sizeof(char) * BUFSIZE);
//    get(params2, body2, ETCD_HOST);
//    printf("body2:%s\n", body2);

//    test_parseHttp();
    test_doRequest();

    return 0;
}

void test_parseHttp() {
    char *str = "HTTP/1.1 200 OK\n"
                "Access-Control-Allow-Headers: accept, content-type, authorization\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, PUT, DELETE\n"
                "Access-Control-Allow-Origin: *\n"
                "Content-Type: application/json\n"
                "X-Etcd-Cluster-Id: cdf818194e3a8c32\n"
                "X-Etcd-Index: 32\n"
                "X-Raft-Index: 41\n"
                "X-Raft-Term: 4\n"
                "Date: Wed, 08 Apr 2020 00:36:25 GMT\n"
                "Content-Length: 793\r\n"
                "\r\n"
                "{\"action\":\"get\",\"node\":{\"key\":\"/registry/pods/nginx2\",\"value\":\"{\\n  \\\"kind\\\": \\\"Pod\\\",\\n  \\\"apiVersion\\\": \\\"v1beta1\\\",\\n  \\\"id\\\": \\\"php\\\",\\n  \\\"desiredState\\\": {\\n    \\\"manifest\\\": {\\n      \\\"version\\\": \\\"v1beta1\\\",\\n      \\\"id\\\": \\\"php\\\",\\n      \\\"containers\\\": [{\\n        \\\"name\\\": \\\"nginx\\\",\\n        \\\"image\\\": \\\"nginx\\\",\\n        \\\"ports\\\": [{\\n          \\\"containerPort\\\": 80,\\n          \\\"hostPort\\\": 8082\\n        }],\\n        \\\"livenessProbe\\\": {\\n          \\\"enabled\\\": true,\\n          \\\"type\\\": \\\"http\\\",\\n          \\\"initialDelaySeconds\\\": 30,\\n          \\\"httpGet\\\": {\\n            \\\"path\\\": \\\"/index.html\\\",\\n            \\\"port\\\": \\\"8082\\\"\\n       ============str\n"
                "   }\\n        }\\n      }]\\n    }\\n  },\\n  \\\"labels\\\": {\\n    \\\"name\\\": \\\"foo\\\"\\n  }\\n}\\n\",\"modifiedIndex\":30,\"createdIndex\":30}}";

    char *body = (char *) malloc(sizeof(char) * BUFSIZE);
    parseHttp(str, body);
    printf("======================body\n%s", body);
}

void test_doRequest() {
    char *protocol = "unix";
    char *method = "GET";
    char *path = "/v1.39/containers/json?filters={\"status\":[\"running\"]}";
    char *data = "";
    Response response = doRequest(protocol, method, path, data);
    char *body = response.body;
    printf("%s\n", body);
}
