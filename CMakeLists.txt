project(opengl_learning)

set(OPENGL_INCLUDE ${CMAKE_SOURCE_DIR}/include)

set(GLFW_LINK ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)

set(GLAD_FILE ${CMAKE_SOURCE_DIR}/src/glad/glad.c)

add_subdirectory(src/clipping)

add_subdirectory(src/scene)

add_subdirectory(src/common)

add_subdirectory(src/ray_tracing)
