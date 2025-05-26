cmake_minimum_required(VERSION 3.20)

project(Chess)

find_package(SFML 2.6.0 COMPONENTS graphics audio REQUIRED)

add_executable(Chess Chess.cpp)

target_link_libraries(Chess PRIVATE sfml-graphics sfml-audio)