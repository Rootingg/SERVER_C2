cmake_minimum_required(VERSION 3.30)
project(ServerC2 C)

set(CMAKE_C_STANDARD 11)

add_executable(ServerC2 main.c
        cli.c
        utils.c
        task.c
        agent.c)
