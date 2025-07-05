cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Chess)

find_package(SFML 3.0.0 COMPONENTS Graphics Audio REQUIRED)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(Chess src/Chess.cpp)

target_link_libraries(Chess PRIVATE SFML::Graphics SFML::Audio)