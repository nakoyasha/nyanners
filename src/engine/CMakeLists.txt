cmake_minimum_required(VERSION 4.1)
project(Nyanners)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(third_party/luau EXCLUDE_FROM_ALL)
add_subdirectory(third_party/sfml EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glm EXCLUDE_FROM_ALL)
add_subdirectory(third_party/fast_obj EXCLUDE_FROM_ALL)
#add_subdirectory(third_party/imgui-sfml EXCLUDE_FROM_ALL)

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
        instances/drawable/TextLabel.cpp
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
        instances/basic/SignalBase.h
        data/UserdataTags.h
        instances/drawable/MeshPart.h
        instances/drawable/MeshPart.cpp
        resources/Shader.cpp
        resources/Shader.h
        instances/world/World.cpp
        instances/world/World.h
        instances/debug/DebugWindow.cpp
        instances/debug/DebugWindow.h
        instances/datatypes/Color3.h
        instances/drawable/Drawable.cpp
        resources/Texture.cpp
        resources/Texture.h
        instances/world/Skybox.cpp
        instances/world/Skybox.h
)

add_library(
        Nyanners.ThirdParty
        third_party/dear-imgui/imgui.cpp
        third_party/dear-imgui/backends/imgui_impl_opengl3.cpp
        third_party/dear-imgui/imgui_demo.cpp
        third_party/dear-imgui/imgui_draw.cpp
        third_party/dear-imgui/imgui_tables.cpp
        third_party/dear-imgui/imgui_widgets.cpp
        third_party/dear-imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(Nyanners.ThirdParty PUBLIC third_party/dear-imgui third_party/dear-imgui/misc/cpp/ third_party/dear-imgui/backends/ third_party/stb/)
target_include_directories(Nyanners PRIVATE third_party/glm/glm)

#target_link_options(Nyanners PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(Nyanners Nyanners.ThirdParty Luau.VM Luau.Require Luau.Compiler SFML::Window SFML::Graphics SFML::Audio stdc++exp glm::glm fast_obj fast_obj_lib)
target_include_directories(Nyanners PUBLIC .)
