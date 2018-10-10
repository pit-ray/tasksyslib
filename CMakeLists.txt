cmake_minimum_required(VERSION 3.0.0)
project(tasksyslib VERSION 0.1.0)

#if you want ".dll", you have to insert "SHARED" like line 6."
add_library(tasksyslib tasksyslib.cpp)
#add_library(tasksyslib SHARED tasksyslib.cpp)