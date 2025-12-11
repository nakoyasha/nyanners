cmake_minimum_required(VERSION 4.0)
project(Nyanners)

add_subdirectory(vendor/luau vendor/libs/luau EXCLUDE_FROM_ALL)
add_subdirectory(vendor/raylib vendor/libs/raylib EXCLUDE_FROM_ALL)

set(CMAKE_C_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)
set(WORKING_DIRECTORY .)
message(RAYLIB_INCLUDE_DIR="${raylib_INCLUDE_DIR}")

set(CURL_LIBRARY vendor/libcurl/lib )
set(CURL_INCLUDE_DIR vendor/libcurl/include )
set(COMMON_INCLUDE engine/ vendor/libcurl/include/ vendor/raylib/src/ vendor/luau/VM/include/ vendor/luau/Compiler/include/ vendor/luau/Require/Runtime/include vendor/luau/Require/Navigator/include vendor/tinyfiledialogs/ vendor/rlimgui vendor/imgui vendor/imgui/misc/cpp/ ${raylib_INCLUDE_DIR})
set(NO_HTTP_SERVICE false)

file(COPY vendor/binary/ DESTINATION ${CMAKE_BINARY_DIR})

if (WIN32)
    add_compile_definitions(WIN32)
    add_compile_definitions(WIN32 WIN32_LEAN_AND_MEAN)
   #  set(NO_HTTP_SERVICE true)
    include_directories(${COMMON_INCLUDE})
endif (WIN32)

set(LOUD_FLAGS "-Wall -Wextra -Weffc++")

if (UNIX)
    include_directories(/usr/local/include ${COMMON_INCLUDE})
endif (UNIX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -masm=intel")
set (CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")

add_library(
    Nyanners.ThirdParty
    vendor/tinyfiledialogs/tinyfiledialogs.c
    vendor/imgui/imgui.cpp 
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp 
    vendor/imgui/imgui_tables.cpp 
    vendor/imgui/imgui_widgets.cpp
        vendor/imgui/misc/cpp/imgui_stdlib.cpp
    vendor/rlimgui/rlImGui.cpp
)

set(
    NYANNERS_SOURCE
    # engine core
        engine/core/Application.cpp
    engine/main.cpp
        engine/core/Logger.cpp
    engine/lua/system.cpp
    engine/lua/utils.cpp
    engine/lua/reflection/Reflection.cpp
    engine/project/ProjectLoader.cpp
    engine/lua/builtin/libInstance.cpp
        engine/datatypes/UDim2.cpp
    engine/instances/Instance.cpp
    engine/instances/DataModel.cpp
    engine/instances/Script.cpp
        engine/services/AssetService.cpp
        engine/instances/ui/TextLabel.cpp
        engine/instances/ui/ImageLabel.cpp
        engine/instances/ui/Button.cpp
        engine/instances/Sound.cpp
)

if (NOT NO_HTTP_SERVICE)
    set(NYANNERS_SOURCE ${NYANNERS_SOURCE} engine/services/HttpService.cpp)
endif (NOT NO_HTTP_SERVICE)

link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug)
link_directories(${CURL_LIBRARY})
add_executable(
    Nyanners
    ${NYANNERS_SOURCE}
)

set_property(TARGET Nyanners PROPERTY INTERPROCEDURAL_GENERATION TRUE)

if (NO_HTTP_SERVICE)
    target_compile_definitions(Nyanners PRIVATE NO_HTTP_SERVICE=1)
endif (NO_HTTP_SERVICE)

set_property(TARGET Nyanners PROPERTY CXX_STANDARD 23)
message("Linking Libraries")

add_definitions( -DCURL_STATICLIB )
target_link_options(Nyanners PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(Nyanners Nyanners.ThirdParty Luau.VM Luau.Require Luau.Compiler raylib libcurl stdc++exp)
