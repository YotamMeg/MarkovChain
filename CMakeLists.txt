cmake_minimum_required(VERSION 3.24)
project(ex3b_yotam267 C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(ex3b_yotam267
        linked_list.c
        linked_list.h
        markov_chain.h
        snakes_and_ladders.c tweets_generator.c markov_chain.c)
