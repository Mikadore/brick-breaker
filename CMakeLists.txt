cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 23)
project(BrickBreaker)

add_executable(game main.cpp)

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
target_link_libraries(game PRIVATE sfml-system sfml-window sfml-graphics)
