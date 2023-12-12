project(opengl_learning)

set(OPENGL_INCLUDE ${CMAKE_SOURCE_DIR}/include)

set(GLFW_LINK ${CMAKE_SOURCE_DIR}/lib/glfw3.lib)
set(ASSIMP_LINK ${CMAKE_SOURCE_DIR}/lib/assimp-vc143-mt.lib)

set(GLAD_FILE ${CMAKE_SOURCE_DIR}/src/glad/glad.c)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("Found OpenMP")
#    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS})
endif()

add_subdirectory(src/clipping)

add_subdirectory(src/scene)

add_subdirectory(src/common)

add_subdirectory(src/ray_tracing)
