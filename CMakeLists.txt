cmake_minimum_required(VERSION 4.1)
project(Nyanners.App)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(src/engine)

add_executable(Nyanners.App src/app/main.cpp
        src/app/debug/ExplorerPanel.cpp
        src/app/debug/ExplorerPanel.h
        src/app/debug/ViewportPanel.cpp
        src/app/debug/ViewportPanel.h
        src/app/debug/CommandBar.cpp
        src/app/debug/CommandBar.h
        src/app/debug/OutputPanel.cpp
        src/app/debug/OutputPanel.h)
target_link_libraries(Nyanners.App PUBLIC Nyanners)
link_libraries(Nyanners.App Nyanners)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Copying resources directory to build directory"
)

add_dependencies(Nyanners.App copy_assets)