cmake_minimum_required(VERSION 4.1)
project(Nyanners)

set(CMAKE_CXX_STANDARD 23)
set(SFML_USE_SYSTEM_DEPS OFF)
set(SFML_BUILD_GRAPHICS OFF)
set(LUAU_BUILD_CLI false)
set(LUAU_BUILD_TESTS false)
set(SFML_BUILD_NETWORK false)
set(LUAU_STATIC_CRT true)


add_subdirectory(third_party/luau EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)
add_subdirectory(third_party/sfml EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glm EXCLUDE_FROM_ALL)
add_subdirectory(third_party/fast_obj EXCLUDE_FROM_ALL)
add_subdirectory(third_party/freetype EXCLUDE_FROM_ALL)
add_subdirectory(third_party/plutosvg EXCLUDE_FROM_ALL)

add_library(Nyanners
        Application.h
        Application.cpp
        core/Logger.h
        core/Logger.cpp
        scripting/data/UserdataTags.h
        instances/basic/SignalBase.h
        instances/basic/Signal.h
        instances/basic/Object.h
        instances/basic/Object.cpp
        instances/datatypes/Color3.h
        instances/datatypes/UDim2.cpp
        instances/drawable/Drawable.h
        instances/drawable/Drawable.cpp
        instances/drawable/MeshPart.h
        instances/drawable/MeshPart.cpp
        instances/ui/TextLabel.h
        instances/ui/TextLabel.cpp
        instances/Instance.h
        instances/Instance.cpp
        instances/DataModel.h
        instances/DataModel.cpp
        instances/Script.h
        instances/Script.cpp
        instances/services/EngineService.h
        instances/services/EngineService.cpp
        instances/services/io/IOService.h
        instances/services/io/IOService.cpp
        instances/services/AssetService.cpp
        instances/services/AssetService.h
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
        core/rendering/Renderer.cpp
        instances/datatypes/Vector.h
        scripting/LibInstance.cpp
        scripting/LibInstance.h
        scripting/reflections/DataTypes.h
        scripting/reflections/DataTypes.cpp
        scripting/reflections/ReflectionEnumRegistry.h
        scripting/reflections/ReflectionMethodTraits.h
        scripting/reflections/ReflectionValueIO.h
        scripting/LibDatatype.cpp
        scripting/LibDatatype.h
        instances/services/ConsoleService.cpp
        instances/services/ConsoleService.h
        instances/services/TextService.cpp
        instances/services/TextService.h
        utils/glCheck.h
        scripting/reflections/ReflectionTypes.h
        instances/services/user/InputService.cpp
        instances/services/user/InputService.h
        resources/Font.cpp
        resources/Font.h
        instances/basic/Transformable.h
        instances/container/BasicContainers.h
        instances/container/BasicContainers.cpp

        debug/DebugUIService.h
        debug/DebugUIService.cpp
        debug/DebugWindow.h
        debug/DebugWindow.cpp
        debug/FrameCounter.cpp
        debug/FrameCounter.h

        debug/ExplorerPanel.cpp
        debug/ExplorerPanel.h
        debug/ViewportPanel.cpp
        debug/ViewportPanel.h
        debug/CommandBar.cpp
        debug/CommandBar.h
        debug/OutputPanel.cpp
        debug/OutputPanel.h
        debug/MainMenubar.cpp
        debug/MainMenubar.h


        scripting/reflections/ReflectionDescriptorRegistry.cpp
        scripting/reflections/ReflectionDescriptorRegistry.h
        scripting/reflections/ReflectionDescriptor.cpp
        scripting/reflections/ReflectionDescriptor.h
        scripting/reflections/ReflectionProperty.h
        scripting/reflections/ReflectionPropertyReaders.h
        utils/ImGuiColor3.h
        scripting/reflections/ReflectionMethod.h
        instances/ui/Button.cpp
        instances/ui/Button.h
        instances/ui/UIElement.h
        instances/ui/LayerCollector.cpp
        instances/ui/LayerCollector.h
        instances/ui/UIElement.cpp
        instances/ui/components/UIComponent.h
        instances/ui/components/UIAspectRatioComponent.cpp
        instances/ui/components/UIAspectRatioComponent.h
        instances/ui/components/UIComponent.cpp
        core/rendering/Viewport.h
        instances/ui/Frame.cpp
        instances/ui/Frame.h
        instances/services/SoundService.cpp
        instances/services/SoundService.h
        instances/world/Sound.cpp
        instances/world/Sound.h
        scripting/reflections/ReflectionEnumRegistry.cpp
        core/Service.h
        instances/services/user/InputObject.h
        instances/services/user/InputObject.cpp
        core/ServiceProvider.cpp
        core/ServiceProvider.h
        scripting/reflections/NativeData.cpp
        scripting/reflections/NativeData.h
        debug/editor/ScriptEditor.cpp
        debug/editor/ScriptEditor.h
        instances/services/io/FileSystem.h
        core/Aliases.h
        instances/drawable/Part.cpp
        instances/drawable/Part.h
        instances/services/light/LightingService.cpp
        instances/services/light/LightingService.h
        instances/services/light/Light.h
        instances/services/light/Light.cpp
)
target_compile_definitions(Nyanners PUBLIC INCLUDE_DEBUG_UI_SERVICE)

add_library(
        Nyanners.ThirdParty
        third_party/dear-imgui/imgui.cpp
        third_party/dear-imgui/misc/freetype/imgui_freetype.cpp
        third_party/dear-imgui/misc/freetype/imgui_freetype.h
        third_party/dear-imgui/backends/imgui_impl_opengl3.cpp
        third_party/dear-imgui/imgui_demo.cpp
        third_party/dear-imgui/imgui_draw.cpp
        third_party/dear-imgui/imgui_tables.cpp
        third_party/dear-imgui/imgui_widgets.cpp
        third_party/dear-imgui/misc/cpp/imgui_stdlib.cpp
)

target_link_libraries(Nyanners.ThirdParty freetype plutosvg::plutosvg)

add_library(
        Nyanners.ThirdParty.Glad
        third_party/glad/src/glad.c
)
target_include_directories(Nyanners.ThirdParty.Glad PUBLIC third_party/glad/include)

add_library(
        Nyanners.ThirdParty.Miniaudio
        third_party/miniaudio/miniaudio.h
        third_party/miniaudio/stb_vorbis.c
)
target_include_directories(Nyanners.ThirdParty.Miniaudio PUBLIC third_party/miniaudio)
set_target_properties(Nyanners.ThirdParty.Miniaudio PROPERTIES LINKER_LANGUAGE CXX)

find_package(Git REQUIRED)

if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
    target_compile_definitions(Nyanners PRIVATE GIT_BRANCH=\"${GIT_BRANCH}\")
else()
    target_compile_definitions(Nyanners PRIVATE GIT_BRANCH=\"Unknown\")
endif()

target_include_directories(Nyanners.ThirdParty PUBLIC third_party/dear-imgui third_party/dear-imgui/misc/cpp/ third_party/dear-imgui/backends/ third_party/dear-imgui/misc/freetype/ third_party/stb/)
target_include_directories(Nyanners PRIVATE third_party/glm/glm)

target_link_options(Nyanners PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(Nyanners Nyanners.ThirdParty Nyanners.ThirdParty.Glad Nyanners.ThirdParty.Miniaudio Luau.VM Luau.Require Luau.Compiler glfw SFML::Window stdc++exp glm::glm freetype fast_obj fast_obj_lib)


#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message("${CMAKE_BUILD_TYPE}")
#    target_compile_definitions(Nyanners PUBLIC INCLUDE_DEBUG_UI_SERVICE=1)
#    target_sources(
#            Nyanners
#            PUBLIC
#            debug/DebugUIService.h
#            debug/DebugUIService.cpp
#            debug/DebugWindow.h
#            debug/DebugWindow.cpp
#            debug/FrameCounter.cpp
#            debug/FrameCounter.h
#
#            debug/ExplorerPanel.cpp
#            debug/ExplorerPanel.h
#            debug/ViewportPanel.cpp
#            debug/ViewportPanel.h
#            debug/CommandBar.cpp
#            debug/CommandBar.h
#            debug/OutputPanel.cpp
#            debug/OutputPanel.h
#            debug/MainMenubar.cpp
#            debug/MainMenubar.h
#    )
#endif()

target_compile_definitions(Nyanners PRIVATE IMGUI_ENABLE_FREETYPE)
target_include_directories(Nyanners PUBLIC .)
