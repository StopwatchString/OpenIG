cmake_minimum_required(VERSION 3.29)

project(          OpenIG VERSION 0.1
                  DESCRIPTION "OpenIG"
                  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE INCLUDES "include/*.h*")

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${SOURCES})
source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${INCLUDES})

add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES} ${GLM_INCLUDES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
        PRIVATE
         "${PROJECT_SOURCE_DIR}/include"
         "${PROJECT_SOURCE_DIR}/include/imgui")

add_subdirectory("extern/glm")
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory("extern/glfw")
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)