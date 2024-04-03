cmake_minimum_required(VERSION 3.22)

project(
    test-server
    VERSION 1.0
    LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

add_subdirectory(src)
add_executable(server src/main.cpp)
add_executable(client test-client/main.cpp)
target_link_libraries(server sandbox)
target_link_libraries(client sandbox)

add_subdirectory(tests)


# cmake --build . -j 