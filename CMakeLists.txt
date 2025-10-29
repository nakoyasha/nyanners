cmake_minimum_required(VERSION 4.1)
project(Nyanners)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake_stuff)

add_subdirectory(vendor/luau vendor/libs/luau)
add_subdirectory(vendor/raylib vendor/libs/raylib)

if (UNIX)
    find_package(raylib REQUIRED)
endif (UNIX)


set(CMAKE_C_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)
# set (CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
message(RAYLIB_INCLUDE_DIR="${raylib_INCLUDE_DIR}")

set(COMMON_INCLUDE engine/ vendor/luau/VM/include/ vendor/luau/Compiler/include/ vendor/tinyfiledialogs/ vendor/rlimgui vendor/imgui ${raylib_INCLUDE_DIR})

if (WIN32)
    add_compile_definitions(WIN32)
    set(raylib_INCLUDE_DIR vendor/raylib/raylib/include)
    include_directories(${COMMON_INCLUDE})
endif (WIN32)

if (UNIX)
    include_directories(/usr/local/include ${COMMON_INCLUDE})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif (UNIX)

add_executable(
    Nyanners 
    # vendor
    vendor/tinyfiledialogs/tinyfiledialogs.c 
    vendor/imgui/imgui.cpp 
    vendor/imgui/imgui_draw.cpp 
    vendor/imgui/imgui_tables.cpp 
    vendor/imgui/imgui_widgets.cpp  
    vendor/rlimgui/rlImGui.cpp
    # engine core
    engine/main.cpp
    engine/lua/system.cpp
    engine/lua/utils.cpp
    engine/Instances/Instance.cpp
    engine/Instances/DataModel.cpp
    engine/Instances/Script.cpp
)
message(RAYLIB_LIBRARIES="${raylib_LIBRARIES}")
message(raylib_LIBRARIES="${raylib_LIBRARIES}")
target_link_libraries(Nyanners ${raylib_LIBRARIES})
message(Linking Luau)
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug)

if (UNIX)
    target_link_libraries(Nyanners libLuau.VM libLuau.Compiler libraylib libLuaBridge)
endif (UNIX)

if (WIN32)
    target_link_libraries(Nyanners Luau.VM Luau.Compiler raylib)
endif (WIN32)