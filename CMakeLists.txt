cmake_minimum_required(VERSION 3.15)
project(c_k8s)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_subdirectory(utils/cJSON)

add_subdirectory(utils/unix-socket)

add_executable(apiServer
        apiServer.c http/http.h http/http.c)

#add_executable(server server.c)
#
#add_executable(client client.c)

#test
add_executable(test_http
        http/test_http.c http/http.h http/http.c
        http/httpClient.h http/httpClient.c)

add_executable(test_etcdClient
        etcd-client/test_etcdClient.c
        etcd-client/etcdClient.c
        etcd-client/etcdClient.h
        http/http.h
        http/http.c
        utils/cJSON/cJSON.h
        utils/cJSON/cJSON.c)

add_executable(test_client
        utils/unix-socket/client.h
        utils/unix-socket/test_client.c
        utils/unix-socket/client.c)

add_executable(test_server
        utils/unix-socket/server.h
        utils/unix-socket/test_server.c
        utils/unix-socket/server.c)

add_executable(test_docker_client
        docker-client/client.h
        docker-client/client.c
        docker-client/test_client.c)