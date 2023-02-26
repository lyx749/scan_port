cmake_minimum_required(VERSION 3.0.0)
project(scan_port VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
include_directories(src/error src/ip src/tcp src/my_thread src/udp)
add_executable(scan_port_by_tcp src/main/main.cpp)
find_package(Threads REQUIRED)
target_link_libraries(scan_port_by_tcp Threads::Threads)
# add_subdirectory(src/udp)


