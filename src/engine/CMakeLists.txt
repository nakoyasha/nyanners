cmake_minimum_required(VERSION 4.1)
project(Nyanners)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(third_party/luau EXCLUDE_FROM_ALL)
add_subdirectory(third_party/sfml EXCLUDE_FROM_ALL)

set(LUAU_BUILD_CLI false)
set(LUAU_BUILD_TESTS false)
set(SFML_BUILD_NETWORK false)

add_library(Nyanners Application.cpp Application.h
        instances/Instance.cpp
        instances/Instance.h
        instances/DataModel.cpp
        instances/DataModel.h
        instances/services/EngineService.cpp
        instances/services/EngineService.h
        instances/Script.cpp
        instances/Script.h
        instances/services/ScriptService.cpp
        instances/services/ScriptService.h
        core/Logger.cpp
        core/Logger.h
        instances/services/IOService.cpp
        instances/services/IOService.h
        instances/drawable/Drawable.h
        instances/drawable/TextLabel.h
        instances/services/ReflectionService.cpp
        instances/services/ReflectionService.cpp
        instances/services/ReflectionService.h
        instances/services/RenderingService.cpp
        instances/services/RenderingService.h
        instances/services/RunService.cpp
        instances/services/RunService.h
        instances/basic/Signal.h
        instances/services/UIService.cpp
        instances/services/UIService.h
)
#target_link_options(Nyanners PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(Nyanners Luau.VM Luau.Require Luau.Compiler SFML::Window SFML::Graphics SFML::Audio stdc++exp)
target_include_directories(Nyanners PUBLIC .)