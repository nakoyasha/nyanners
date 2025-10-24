cmake_minimum_required(VERSION 4.1)
project(Nyanners)

find_package(Lua REQUIRED) 
find_package(raylib REQUIRED)

set(CMAKE_C_STANDARD 17)
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include_directories(/usr/local/include ${LUA_INCLUDE_DIR} vendor/luau/VM/include/ vendor/luau/Compiler/include/ ${RAYLIB_INCLUDE_DIR})

add_executable(Nyanners engine/engine.cpp)
message(LUA_LIBRARIES="${LUA_LIBRARIES}")
target_link_libraries(Nyanners ${LUA_LIBRARIES})
message(RAYLIB_LIBRARIES="${raylib_LIBRARIES}")
target_link_libraries(Nyanners ${raylib_LIBRARIES})
message(Linking Luau)
target_link_libraries(Nyanners ${CMAKE_SOURCE_DIR}/vendor/libs/luau/libLuau.VM.a ${CMAKE_SOURCE_DIR}/vendor/libs/luau/libLuau.Compiler.a ${CMAKE_SOURCE_DIR}/vendor/libs/luau/libLuau.Ast.a)