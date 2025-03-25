cmake_minimum_required(VERSION 3.30)
project(MonicaImageProcess_Server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# boost
find_package(Boost COMPONENTS serialization system filesystem program_options thread REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})