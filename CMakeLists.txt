cmake_minimum_required(VERSION 3.20)
project(ComputerClub)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(computer_club
    main.cpp
    event.cpp
    computer_club.cpp
)

target_include_directories(computer_club PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})