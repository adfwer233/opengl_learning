#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <iostream>
#include <filesystem>
#include <ranges>
#include <fstream>
#include <array>

#include "common/camera/camera.hxx"
#include "common/constructor/constructor.hxx"
#include "common/mesh_model.hxx"
#include "common/shader.hxx"
#include "common/skybox/skybox.h"
#include "common/simulation/solid_entity.hxx"
#include "common/io/model_io.h"
#include "common/io/render_output.h"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader"
#endif

#ifndef SKYBOX_DIR
#define SKYBOX_DIR "./skybox"
#endif

#ifndef TEXTURE_DIR
#define TEXTURE_DIR "./texture"
#endif

#ifndef MODEL_DIR
#define MODEL_DIR "./model"
#endif

bool enable_filter = false;

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

constexpr unsigned int SHADOW_WIDTH = 2048;
constexpr unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.5, 0.5, 5.0f), glm::vec3(0, 1.0f, 0));

constexpr const int n = 480;
constexpr const int m = 600;
std::array<std::array<glm::vec3, m>, n> image;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.process_keyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.process_keyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        enable_filter = false;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        enable_filter = true;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                image[i][j] = {float(i) / n, float(j) / m, 0.2};
            }
        }

        output_ppm_image(image);
        std::cout << "output finish" << std::endl;
    }

}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    camera.process_mouse_scroll(y_offset);
}

bool is_mouse_pressing = false;
bool mouse_flag = true;

void mouse_button_callback(GLFWwindow* window, int button, int state, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS)
        is_mouse_pressing = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_RELEASE) {
        mouse_flag = true;
        is_mouse_pressing = false;
    }
}


float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (not is_mouse_pressing) return;

    float x_pos = static_cast<float>(xposIn);
    float y_pos = static_cast<float>(yposIn);

    if (mouse_flag) {
        last_x = x_pos;
        last_y = y_pos;
        mouse_flag = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

    last_x = x_pos;
    last_y = y_pos;

    camera.process_mouse_movement(x_offset, y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {

    std::string config_path_str = "";

    std::string shader_root = SHADER_DIR;
    std::string skybox_root = SKYBOX_DIR;

    if (argc >= 2) {
        config_path_str = std::string(argv[1]);
    }

    if (argc >= 3) shader_root = std::string(argv[2]);
    if (argc >= 4) skybox_root = std::string(argv[3]);

    std::cout << std::format("config path: {}\n", config_path_str);

    std::filesystem::path config_path(config_path_str);

    std::fstream config_fstream;
    config_fstream.open(config_path_str, std::ios::in);
    int config_ent_num = 0;
    config_fstream >> config_ent_num;
    std::cout << config_ent_num << std::endl;


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scene Rendering", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader(std::format("{}/lighting.vs", shader_root), std::format("{}/lighting.fs", shader_root));
    Shader shadow_map_shader(std::format("{}/shadow.vs", shader_root), std::format("{}/shadow.fs", shader_root));
    Shader screenShader(std::format("{}/filter.vs", shader_root), std::format("{}/filter.fs", shader_root));

    // Frame Buffer Binding for shadow map
    GLuint depth_map_FBO;
    glGenFramebuffers(1, &depth_map_FBO);

    GLuint depth_map;
    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Frame buffer binding for filter
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // frame buffer for mirror


    // set light position
    glm::vec3 lightPos(-7, 7, 10);

    MeshModel sphere = Constructor::Sphere(Point3d(1.5, 0, 0), 0.2);
    MeshModel sphere2 = Constructor::Sphere(Point3d(-0.5, -0.5, 1), 0.2);
    MeshModel light_src = Constructor::Sphere(Point3d(lightPos.x, lightPos.y, lightPos.z), 0.1);
    MeshModel cubic = Constructor::Cubic({ -0.3, -0.3, -0.3 }, { 0.3, 0.3, 0.3 });
    MeshModel cubic2 = Constructor::Cubic({ -1.6, -0.3, -1.6 }, { -1.3, 0.3, -1.3 });

    MeshModel mirror = Constructor::Rectangle({-1, -1, 1}, {-1, 1, 1}, {1, -1, 1});
    MeshModel mirror2 = Constructor::Rectangle({-0.5, -0.5, 1.5}, {-0.5, 0.5, 1.5}, {0.5, -0.5, 1.5});

    int entity_count = 0;

    std::vector<std::reference_wrapper<MeshModel>> mesh_models{ sphere, sphere2, cubic2, mirror, mirror2 };
    std::ranges::for_each(mesh_models, [](std::reference_wrapper<MeshModel> &x){x.get().bind_buffer();});

    cubic.bind_buffer();

    cubic2.bind_texture(std::format("{}/container.jpg", TEXTURE_DIR), diffuse_texture);
    mirror.bind_texture_with_alpha(std::format("{}/mirror.png", TEXTURE_DIR), diffuse_texture);
    mirror2.bind_texture_with_alpha(std::format("{}/mirror.png", TEXTURE_DIR), diffuse_texture);

    auto mesh_from_obj = ModelIO().read_obj_model(std::format("{}/nanosuit/nanosuit.obj", MODEL_DIR));

    std::ranges::for_each(mesh_from_obj, [](auto &x){ x.bind_buffer(); });
    std::cout << mesh_from_obj.size()  << std::endl;

//    sphere.bind_texture(std::format("{}/container.jpg", TEXTURE_DIR));

//    std::ranges::for_each(mesh_models, [](auto x){ x.get().bind_texture(std::format("{}/container.jpg", TEXTURE_DIR)); });

    light_src.bind_buffer();

    // sky box initialization
    std::vector<std::string> faces
            {
                    std::format("{}/right.jpg", skybox_root),
                    std::format("{}/left.jpg", skybox_root),
                    std::format("{}/top.jpg", skybox_root),
                    std::format("{}/bottom.jpg", skybox_root),
                    std::format("{}/front.jpg", skybox_root),
                    std::format("{}/back.jpg", skybox_root)
            };

    SkyBox skybox;
    skybox.load_cube_map(faces);
    skybox.bind();

    Shader skybox_shader(std::format("{}/simple.vs", skybox_root), std::format("{}/simple.fs", skybox_root));
    skybox_shader.use();
    skybox_shader.set_int("skybox", 0);

    screenShader.use();
    screenShader.set_int("screenTexture", 0);

    shader.use();
    shader.set_int("shadowMap", 0);
    shader.set_int("diffuseTexture", 1);
    shader.set_int("specularTexture", 1);

    Shader envir_reflect_shader(std::format("{}/envir_reflect.vs", shader_root), std::format("{}/envir_reflect.fs", shader_root));
    envir_reflect_shader.use();
    envir_reflect_shader.set_int("skybox", 0);

    while (not glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 100.5f;
        lightProjection = glm::perspective(glm::radians(camera.zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        shadow_map_shader.use();
        shadow_map_shader.set_mat4("lightSpaceMatrix", lightSpaceMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1, 0.1, 0.1, 1);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // step1: render the shadow map

        glClear(GL_DEPTH_BUFFER_BIT);
        std::ranges::for_each(mesh_models, [&](std::reference_wrapper<MeshModel> model){ model.get().process_shadow_rendering(shadow_map_shader); });
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // step2: render the scene

        shader.use();
        shader.set_mat4("lightSpaceMatrix", lightSpaceMatrix);

        if (enable_filter)
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glDepthMask(GL_LEQUAL);
        skybox_shader.use();
        unsigned int transformLoc = glGetUniformLocation(skybox_shader.ID, "model");
        auto scale = glm::mat4(1.0f);
        scale = glm::scale(scale, glm::vec3(10.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(scale));
        unsigned int viewTransformLoc = glGetUniformLocation(skybox_shader.ID, "view");
        glUniformMatrix4fv(viewTransformLoc, 1, GL_FALSE, glm::value_ptr(camera.get_view_transformation()));
        unsigned int projectionTransformLoc = glGetUniformLocation(skybox_shader.ID, "projection");
        projection = glm::perspective(glm::radians(camera.zoom), 1.0f * 1024 / 1024, 0.1f, 100.0f);
        glUniformMatrix4fv(projectionTransformLoc, 1, GL_FALSE, glm::value_ptr(projection));
        skybox.rendering();
        glDepthMask(GL_LESS);

        envir_reflect_shader.use();
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
        cubic.process_environment_reflection_rendering(envir_reflect_shader, camera, skybox.ID);

        std::ranges::for_each(mesh_from_obj, [&](auto &x){ x.process_rendering(shader, camera, depth_map, lightPos); });
        std::ranges::sort(mesh_models, [](auto &x, auto &y){ return x.get().get_distance(camera.position) < y.get().get_distance(camera.position); });
        std::ranges::for_each(mesh_models, [&](std::reference_wrapper<MeshModel> model){ model.get().process_rendering(shader, camera, depth_map, lightPos); });
        light_src.process_rendering(shader, camera, depth_map, lightPos, glm::vec3(10, 10, 10));

        if (enable_filter) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            screenShader.use();
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;

}
