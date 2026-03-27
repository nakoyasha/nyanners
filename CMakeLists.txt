cmake_minimum_required(VERSION 4.1)
project(Nyanners.App)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src/engine)

add_executable(Nyanners.App src/app/main.cpp)
target_link_libraries(Nyanners.App PUBLIC Nyanners)
link_libraries(Nyanners.App Nyanners)
