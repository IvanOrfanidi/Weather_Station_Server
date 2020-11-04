# Version CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(weather-station-server VERSION 1.0.10)

configure_file(
    project_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/project_version.h
)

# Option
set(CMAKE_CXX_STANDARD 17)

# Compiler-specific options
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wcast-align
        -Wcast-qual
        -Wenum-compare
        -Wfloat-equal
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-promo
    )
endif()

# Source files
file(GLOB_RECURSE SOURCES
    source/*.cpp
    source/*.h
    source/utils/*.h
)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Compiling target
add_executable(${PROJECT_NAME} ${SOURCES})

if(UNIX)
    find_package(Threads)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

# Link lib TCP/UDP server, client
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/tcp_udp_server/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/tcp_udp_server/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/tcp_udp_client/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/tcp_udp_client/lib)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/build/tcp_udp_server/lib/lib_tcp_udp_server.a")
    target_link_libraries(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/build/tcp_udp_client/lib/lib_tcp_udp_client.a")
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/build/tcp_udp_server/lib/lib_tcp_udp_server.a")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/build/tcp_udp_client/lib/lib_tcp_udp_client.a")
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# Link target
target_link_libraries(${PROJECT_NAME})
