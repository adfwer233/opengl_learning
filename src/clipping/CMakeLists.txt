project(clipping CXX C)

set(SRC_FILE "main.cpp")

include_directories(${OPENGL_INCLUDE})

LINK_LIBRARIES(${GLFW_LINK})

add_executable(${PROJECT_NAME} ${SRC_FILE} ${GLAD_FILE})