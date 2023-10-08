project(common CXX C)

set(SOURCE_FILE "shader.cpp")

include_directories(${OPENGL_INCLUDE})

LINK_LIBRARIES(${GLFW_LINK})

add_library(${PROJECT_NAME} ${SOURCE_FILE} ${GLAD_FILE})