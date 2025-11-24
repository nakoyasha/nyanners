cmake_minimum_required(VERSION 4.1)
project(Nyanners)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake_stuff)

add_subdirectory(vendor/luau vendor/libs/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib vendor/libs/raylib EXCLUDE_FROM_ALL)
set(CMAKE_BUILD_TYPE Debug)

# if (UNIX)
#     find_package(raylib REQUIRED)
# endif (UNIX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)
set(WORKING_DIRECTORY .)
# set (CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
message(RAYLIB_INCLUDE_DIR="${raylib_INCLUDE_DIR}")

set(COMMON_INCLUDE engine/ vendor/raylib/src/ vendor/luau/VM/include/ vendor/luau/Compiler/include/ vendor/tinyfiledialogs/ vendor/rlimgui vendor/imgui ${raylib_INCLUDE_DIR})

if (WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(NOGDI)
    add_compile_definitions(NOCRYPT)
    set (CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
    include_directories(${COMMON_INCLUDE})
endif (WIN32)

set(LOUD_FLAGS "-Wall -Wextra -Weffc++")

if (UNIX)
    include_directories(/usr/local/include ${COMMON_INCLUDE})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g3 -march=native -masm=intel -std=c++23")
endif (UNIX)

add_library(httplib INTERFACE)
target_include_directories(httplib INTERFACE engine/http)

add_library(
    Nyanners.ThirdParty
    vendor/tinyfiledialogs/tinyfiledialogs.c 
    vendor/imgui/imgui.cpp 
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp 
    vendor/imgui/imgui_tables.cpp 
    vendor/imgui/imgui_widgets.cpp  
    vendor/rlimgui/rlImGui.cpp
)


add_executable(
    Nyanners
    # engine core
    engine/main.cpp
    engine/lua/system.cpp
    engine/lua/utils.cpp
    engine/lua/builtin/libInstance.cpp
    engine/instances/Instance.cpp
    engine/instances/DataModel.cpp
    engine/instances/Script.cpp
    engine/instances/TextLabel.cpp
    engine/services/HttpService.cpp
)


set_property(TARGET Nyanners PROPERTY CXX_STANDARD 23)
message(RAYLIB_LIBRARIES="${raylib_LIBRARIES}")
message(raylib_LIBRARIES="${raylib_LIBRARIES}")
target_link_libraries(Nyanners ${raylib_LIBRARIES} Nyanners.ThirdParty)
message("Linking Libraries")
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug)

target_link_libraries(Nyanners Luau.VM Luau.Compiler httplib raylib ssl crypto)