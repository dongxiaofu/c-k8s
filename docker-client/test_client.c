//
// Created by chugang on 2020/4/8.
//

#include "client.h"

int main(){
//    printf("hello");
    char *resp = createContainer();
    printf("=============resp\n%s\n\n", resp);
    printf("len = %lu\n", strlen(resp));

}