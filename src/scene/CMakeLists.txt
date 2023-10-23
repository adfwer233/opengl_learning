project(scene CXX C)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE "main.cpp")

add_definitions(-DSHADER_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}/shader\\")

include_directories(${OPENGL_INCLUDE})

LINK_LIBRARIES(${GLFW_LINK})

add_executable(${PROJECT_NAME} ${SRC_FILE} ${GLAD_FILE})

target_link_libraries(${PROJECT_NAME} common)
