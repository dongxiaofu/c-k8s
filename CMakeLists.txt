cmake_minimum_required(VERSION 3.15)
project(c_k8s)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(apiServer
        apiServer.c)

add_executable(server server.c)

add_executable(client client.c)