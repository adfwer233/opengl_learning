#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <iostream>
#include "common/constructor/constructor.hxx"
#include "common/mesh_model.hxx"
#include "common/shader.hxx"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader"
#endif

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

int main() {

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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE);

    MeshModel sphere = Constructor::Sphere(Point3d(0, 0 ,0), 0.2);
    MeshModel sphere2 = Constructor::Sphere(Point3d(-0.5, -0.5, 0.5), 0.2);

    unsigned int VBO[3], VAO[3], EBO[3];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

	auto bind_mesh = [&](int index, MeshModel &model) {
		glBindVertexArray(VAO[index]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);

		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Point3d), model.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces_indices.size() * sizeof(TriangleVerticeIndex), model.faces_indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	};

    Shader shader(std::format("{}/simple.vs", SHADER_DIR), std::format("{}/green.fs", SHADER_DIR));
	view = glm::translate(view, glm::vec3(0, 0, -3.0));
	projection = glm::perspective(glm::radians(45.0f), 1.0f * SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

	auto processMeshModel = [&](int index, MeshModel &model) {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(model.transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        glUseProgram(shader.ID);

        unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        unsigned int viewTransformLoc = glGetUniformLocation(shader.ID, "view");
        glUniformMatrix4fv(viewTransformLoc, 1, GL_FALSE, glm::value_ptr(view));

		unsigned int projectionTransformLoc = glGetUniformLocation(shader.ID, "projection");
        glUniformMatrix4fv(projectionTransformLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO[index]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, model.faces_indices.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
	};

	bind_mesh(0, sphere);
	bind_mesh(1, sphere2);

    std::cout << "Construct finish " << std::endl;


    while (not glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.9, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		processMeshModel(0, sphere);
		processMeshModel(1, sphere2);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;

}
