cmake_minimum_required(VERSION 4.1)
project(Nyanners.App)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(src/engine)

add_executable(Nyanners.App src/app/main.cpp)
target_link_libraries(Nyanners.App PUBLIC Nyanners)
link_libraries(Nyanners.App Nyanners)

add_custom_command(
        TARGET Nyanners.App
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Copying resources directory to build directory"
)
