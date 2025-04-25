cmake_minimum_required(VERSION 3.20...3.31)
project(JBRenderer
    LANGUAGES CXX
    DESCRIPTION "A vulkan renderer"
    VERSION 1.0.0)

# search for Vulkan SDK
find_package(Vulkan)

include(FetchContent)
FetchContent_Declare (
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "")
set(GLFW_INSTALL OFF CACHE BOOL  "")
FetchContent_MakeAvailable(glfw)
FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v1.4.307 #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

add_executable(JBRenderer "src/main.cpp")
target_link_libraries(JBRenderer
    PRIVATE
        Vulkan::Vulkan
        glfw
        vk-bootstrap
        vk-bootstrap-compiler-warnings)

# If you're on macOS, you might also need to explicitly link MoltenVK
if(APPLE)
    # You might need to adjust this path based on your Vulkan SDK installation
    target_link_libraries(JBRenderer 
        PRIVATE 
        "/usr/local/lib/libMoltenVK.dylib"
    )
endif()

target_include_directories(JBRenderer PRIVATE ${CMAKE_CURRENT_BINARY_DIR}) # path to build directory for shaders

find_program(GLSLANG_FOUND glslang)
if(GLSLANG_FOUND)
    message(STATUS "glslang found, will compile shaders automatically")

    set(COMPILED_SHADER_FILES)
    macro(compile_shader SHADER_NAME)
        set(SHADER_SOURCE ${CMAKE_SOURCE_DIR}/src/shaders/${SHADER_NAME})
        set(SHADER_SPIRV_NAME ${SHADER_NAME}.spv)
        set(SHADER_SPIRV_PATH ${CMAKE_SOURCE_DIR}/src/shaders/${SHADER_SPIRV_NAME})
        set(SHADER_DEST_SPIRV ${CMAKE_BINARY_DIR}/${SHADER_SPIRV_NAME})

        add_custom_command(
            OUTPUT ${SHADER_SPIRV_PATH}
            COMMAND glslang -V ${SHADER_SOURCE} -o ${SHADER_SPIRV_PATH} --target-env vulkan1.0
            DEPENDS ${SHADER_SOURCE}
            COMMENT "Shader ${SHADER_NAME} compiled"
        )

        add_custom_command(
            OUTPUT ${SHADER_DEST_SPIRV}
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_SPIRV_PATH} ${SHADER_DEST_SPIRV}
            DEPENDS ${SHADER_SPIRV_PATH}
        )
        list(APPEND COMPILED_SHADER_FILES ${SHADER_DEST_SPIRV})
    endmacro()

    compile_shader(triangle.frag)
    compile_shader(triangle.vert)
    #compile_shader(simple_compute.comp)
    add_custom_target(generate_shaders DEPENDS ${COMPILED_SHADER_FILES})
    add_dependencies(JBRenderer generate_shaders)
else()
    message(STATUS "glslang not found! Will need to add it.")
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/src/example_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/example_config.h"
)