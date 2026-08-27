cmake_minimum_required(VERSION 4.1)
project(Nyanners.App)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(src/engine)
add_subdirectory(src/ci_luau_types)

add_executable(Nyanners.App src/app/main.cpp)
target_link_libraries(Nyanners.App PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,Nyanners>")
link_libraries(Nyanners.App Nyanners)

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
        COMMENT "Copying resources directory to build directory"
)

add_custom_target(copy_examples ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        "${CMAKE_BINARY_DIR}/examples"
        COMMENT "Copying examples directory to build directory"
)

target_link_options(Nyanners.App PRIVATE -static-libgcc -static-libstdc++)
add_dependencies(Nyanners.App copy_assets copy_examples)