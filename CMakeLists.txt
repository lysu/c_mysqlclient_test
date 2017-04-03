cmake_minimum_required(VERSION 3.7)
project(mysql_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(MySQL REQUIRED)

include_directories(${MYSQL_INCLUDE_DIR})

set(SOURCE_FILES main.c)
add_executable(test_mysql ${SOURCE_FILES})
target_link_libraries(test_mysql ${MYSQL_LIBRARIES})
