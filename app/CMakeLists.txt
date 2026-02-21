cmake_minimum_required(VERSION 3.5)
project(NyannersApp)

add_subdirectory(../engine ${CMAKE_BINARY_DIR}/build/engine EXCLUDE_FROM_ALL)

add_executable(
	App
	main.cpp
)

#include_directories(../engine )
set_property(TARGET App PROPERTY CXX_STANDARD 23)
target_link_options(App PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(App PRIVATE Nyanners)