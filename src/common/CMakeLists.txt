project(common CXX C)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILE "shader.cpp" "intersector.cpp" "polygon.cpp" "point.cpp" "containment.cpp"
        constructor/constructor.cpp)

include_directories(${OPENGL_INCLUDE})

LINK_LIBRARIES(${GLFW_LINK})

add_library(${PROJECT_NAME} ${SOURCE_FILE} ${GLAD_FILE})