project(ray_tracing CXX C)

set(CMAKE_CXX_STANDARD 20)

set(SRC_FILE main.cpp)

add_definitions(-DSHADER_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}/shader\\")
add_definitions(-DSKYBOX_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}/skybox\\")
add_definitions(-DTEXTURE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}/texture\\")
add_definitions(-DMODEL_DIR=\\"${CMAKE_SOURCE_DIR}/assets/models\\")

include_directories(${OPENGL_INCLUDE})

LINK_LIBRARIES(${GLFW_LINK})

add_executable(${PROJECT_NAME} ${SRC_FILE} ${GLAD_FILE})

target_link_libraries(${PROJECT_NAME} common)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/3rdparty/bin/" $<TARGET_FILE_DIR:${PROJECT_NAME}>
)