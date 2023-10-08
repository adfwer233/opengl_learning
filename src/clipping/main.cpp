#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <common/shader.hxx>

#include <iostream>
#include <array>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#ifndef SHADER_DIR
#define SHADER_DIR "./shader"
#endif

class VerticesBuffer {
public:
	std::array<float, 1000> vertices;
	int count = 0;
	VerticesBuffer() = default;

	VerticesBuffer(std::initializer_list<std::initializer_list<float>> init_list) {
		for (auto list : init_list) {
			if (list.size() != 3) {
				std::cout << "ERROR" << std::endl;
				return;
			}

			for (int i = 0; auto item : list) {
				vertices[3 * count + i] = item;
				i++;
			}

			count++;
		}
	}

	float* get_buffer_address() {
		return vertices.data();
	}

	int size() {
		return vertices.size();
	}

	void add_vertices(float x, float y, float z) {
		vertices[count * 3] = x;
		vertices[count * 3 + 1] = y;
		vertices[count * 3 + 2] = z;
		count++;
	}
};

VerticesBuffer vertices1{
	{-0.5f, -0.5f, 0.0f},
	{0.5f,  -0.5f, 0.0f},
	{-0.5f,  0.5f, 0.0f},
	{0.5f,   0.5f, 0.0f},
};

VerticesBuffer vertices2{
	{-0.2f, -0.2f, 0.0f},
	{ 0.2f, -0.2f, 0.0f},
	{ 0.0f,  0.2f, 0.0f}
};

unsigned int VBO[2], VAO[2];

int main()
{
	std::cout << SHADER_DIR << std::endl;
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE);

	// build and compile our shader program
	// ------------------------------------
	// vertex shader

	Shader redShader(std::string(SHADER_DIR) + "/simple.vs", std::string(SHADER_DIR) + "/red.fs");
	Shader greenShader(std::string(SHADER_DIR) + "/simple.vs", std::string(SHADER_DIR) + "/green.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------


	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.count * 4, vertices1.get_buffer_address(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.count * 4, vertices2.get_buffer_address(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
		glUseProgram(redShader.ID);
		glBindVertexArray(VAO[0]); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glDrawArrays(GL_POINTS, 0, vertices1.count);
		glDrawArrays(GL_LINE_STRIP, 0, 4);
		glBindVertexArray(0);

		glUseProgram(greenShader.ID);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		vertices1.add_vertices(0.6, 0.6, 0);
		glBindVertexArray(VAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices1.count * 4, vertices1.get_buffer_address(), GL_STATIC_DRAW);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}