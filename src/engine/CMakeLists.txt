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


add_library(Nyanners
        Application.h
        Application.cpp
        core/Logger.h
        core/Logger.cpp
        data/UserdataTags.h
        instances/basic/SignalBase.h
        instances/basic/Signal.h
        instances/basic/WorldObject.h
        instances/basic/WorldObject.cpp
        instances/datatypes/Color3.h
        instances/drawable/Drawable.h
        instances/drawable/Drawable.cpp
        instances/drawable/MeshPart.h
        instances/drawable/MeshPart.cpp
        instances/drawable/TextLabel.h
        instances/drawable/TextLabel.cpp
        instances/debug/DebugUIService.h
        instances/debug/DebugUIService.cpp
        instances/debug/DebugWindow.h
        instances/debug/DebugWindow.cpp
        instances/Instance.h
        instances/Instance.cpp
        instances/DataModel.h
        instances/DataModel.cpp
        instances/Script.h
        instances/Script.cpp
        instances/services/EngineService.h
        instances/services/EngineService.cpp
        instances/services/IOService.h
        instances/services/IOService.cpp
        instances/services/ReflectionService.h
        instances/services/ReflectionService.cpp
        instances/services/RenderingService.h
        instances/services/RenderingService.cpp
        instances/services/RunService.h
        instances/services/RunService.cpp
        instances/services/ScriptService.h
        instances/services/ScriptService.cpp
        instances/services/SelectionService.h
        instances/services/SelectionService.cpp
        instances/services/UIService.h
        instances/services/UIService.cpp
        instances/world/World.h
        instances/world/World.cpp
        instances/world/Skybox.h
        instances/world/Skybox.cpp
        resources/FrameBuffer.h
        resources/FrameBuffer.cpp
        resources/Shader.h
        resources/Shader.cpp
        resources/Texture.h
        resources/Texture.cpp
        core/rendering/OpenGLRenderer.cpp
        core/rendering/OpenGLRenderer.h
        core/rendering/Renderer.h
        core/rendering/RenderQueue.h
        resources/Mesh.cpp
        resources/Mesh.h
        resources/opengl/OpenGLMesh.cpp
        resources/opengl/OpenGLMesh.h
        resources/IndexBuffer.cpp
        resources/IndexBuffer.h
        instances/datatypes/Vertices.h
        resources/opengl/OpenGLIndexBuffer.cpp
        resources/opengl/OpenGLIndexBuffer.h
        resources/VertexBuffer.cpp
        resources/VertexBuffer.h
        resources/opengl/OpenGLVertexBuffer.cpp
        resources/opengl/OpenGLVertexBuffer.h
        resources/opengl/OpenGLTexture.cpp
        resources/opengl/OpenGLTexture.h
        instances/Camera.cpp
        instances/Camera.h
        resources/Material.cpp
        resources/Material.h
        resources/opengl/OpenGLMaterial.cpp
        resources/opengl/OpenGLMaterial.h
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
