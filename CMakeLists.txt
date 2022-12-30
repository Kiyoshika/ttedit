cmake_minimum_required(VERSION 3.10.0)
project(TTEdit C)

set(CMAKE_C_STANDARD 99)

option(RELEASE_MODE OFF)

if(RELEASE_MODE)
	set(CMAKE_C_FLAGS "-O2 -Wall")
else()
	set(CMAKE_C_FLAGS "-Og -Wall")
endif()

add_subdirectory(src)
