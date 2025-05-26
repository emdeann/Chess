cmake_minimum_required(VERSION 3.20)

project(Chess)

find_package(SFML 2.6.0 COMPONENTS graphics audio REQUIRED)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(Chess src/Chess.cpp)

target_link_libraries(Chess PRIVATE sfml-graphics sfml-audio)