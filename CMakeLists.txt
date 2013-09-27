cmake_minimum_required(VERSION 2.8)
project(irt)
add_executable(irt irt.cpp)
target_link_libraries(irt wiringPi)