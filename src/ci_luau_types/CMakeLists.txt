cmake_minimum_required(VERSION 4.1)
project(Nyanners.CI.TypeBuilder)

add_executable(TypeBuilder main.cpp)
target_link_libraries(TypeBuilder Nyanners)

target_link_options(TypeBuilder PRIVATE -static-libgcc -static-libstdc++)

add_custom_target(copy_typebuilder_autorun ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/autorun.luau"
        "${CMAKE_BINARY_DIR}/autorun.luau"
        COMMENT "autorun.luau"
)

add_dependencies(TypeBuilder copy_typebuilder_autorun)