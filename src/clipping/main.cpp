#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "clipping.hxx"
#include "common/polygon.hxx"
#include "common/shader.hxx"
#include "common/intersector.hxx"

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
		return count * 3 * sizeof(float);
	}

	void add_vertices(float x, float y, float z) {
		vertices[count * 3] = x;
		vertices[count * 3 + 1] = y;
		vertices[count * 3 + 2] = z;
		count++;
	}

	std::tuple<float, float, float> get_vertex(int index) {
		return std::make_tuple(vertices[3 * index], vertices[3 * index + 1], vertices[3 * index + 2]);
	}

	auto get_vertex_tuple_vector() {
		std::vector<std::tuple<float, float, float>> res;
		for (int i: std::views::iota(0, count))
			res.push_back(get_vertex(i));
		return res;
	}

	auto get_vertex_tuple_vector(int begin, int end) {
		std::vector<std::tuple<float, float, float>> res;
		for (int i: std::views::iota(begin, end))
			res.push_back(get_vertex(i));
		return res;
	}
};

VerticesBuffer vertices1{
	// {-0.5f, -0.5f, 0.0f},
	// {0.5f,  -0.5f, 0.0f},
	// {-0.5f,  0.5f, 0.0f},
	// {0.5f,   0.5f, 0.0f},
};

VerticesBuffer vertices2{
	// {-0.2f, -0.2f, 0.0f},
	// { 0.2f, -0.2f, 0.0f},
	// { 0.0f,  0.2f, 0.0f}
};

VerticesBuffer inter_vertices {};

unsigned int VBO[3], VAO[3];

// 0 for first polygon, 1 for second polygon
int current_state = 0;
bool is_inner_state = false;
bool algo_flag = false;

Polygon polygon1, polygon2;
std::vector<int> poly1_flags{0}, poly2_flags{0}, inter_flag{0};

void mouse_button_callback(GLFWwindow* window, int button, int state, int mod) {
	int height, width;
	glfwGetWindowSize(window, &width, &height);

	if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS) {
		std::cout << poly1_flags.back() << ' ' << vertices1.count << std::endl;
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		std::cout << x / width << ' ' << y / height << ' ' << width << ' ' << height << std::endl;

		if (current_state == 0) {
			vertices1.add_vertices(x / width * 2 - 1, 1 - y / height * 2, 0);
			glBindVertexArray(VAO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices1.size(), vertices1.get_buffer_address(), GL_STATIC_DRAW);
		}
		else if (current_state == 1) {
			vertices2.add_vertices(x / width * 2 - 1, 1 - y / height * 2, 0);
			glBindVertexArray(VAO[1]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, vertices2.size(), vertices2.get_buffer_address(), GL_STATIC_DRAW);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_PRESS) {
		if (current_state == 0) {
			auto [x, y, z] = vertices1.get_vertex(poly1_flags.back());
			vertices1.add_vertices(x, y, z);
			glBindVertexArray(VAO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, vertices1.size(), vertices1.get_buffer_address(), GL_STATIC_DRAW);

			polygon1.add_loop(vertices1.get_vertex_tuple_vector(poly1_flags.back(), vertices1.count - 1));

			poly1_flags.push_back(vertices1.count);
			polygon1.print_polygon_message();

		}
		else if (current_state == 1) {
			auto [x, y, z] = vertices2.get_vertex(poly2_flags.back());
			vertices2.add_vertices(x, y, z);
			glBindVertexArray(VAO[1]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, vertices2.size(), vertices2.get_buffer_address(), GL_STATIC_DRAW);

			polygon2.add_loop(vertices2.get_vertex_tuple_vector(poly2_flags.back(), vertices2.count - 1));

			poly2_flags.push_back(vertices2.count);
			polygon2.print_polygon_message();
		}
	}
		
}

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
	Shader blueShader(std::string(SHADER_DIR) + "/simple.vs", std::format("{}/blue.fs", SHADER_DIR));

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size(), vertices1.get_buffer_address(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size(), vertices2.get_buffer_address(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, inter_vertices.size(), inter_vertices.get_buffer_address(), GL_STATIC_DRAW);
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
		for (int i = 1; i < poly1_flags.size(); i++)
			glDrawArrays(GL_LINE_STRIP, poly1_flags[i - 1], poly1_flags[i] - poly1_flags[i - 1]);
		if (poly1_flags.back() < vertices1.count - 1)
			glDrawArrays(GL_LINE_STRIP, poly1_flags.back(), vertices1.count - poly1_flags.back());

		glBindVertexArray(0);

		glUseProgram(greenShader.ID);
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_POINTS, 0, vertices2.count);

		for (int i = 1; i < poly2_flags.size(); i++)
			glDrawArrays(GL_LINE_STRIP, poly2_flags[i - 1], poly2_flags[i] - poly2_flags[i - 1]);
		if (poly2_flags.back() < vertices2.count - 1)
			glDrawArrays(GL_LINE_STRIP, poly2_flags.back(), vertices2.count - poly2_flags.back());

		glBindVertexArray(0);

		glUseProgram(blueShader.ID);
		glBindVertexArray(VAO[2]);
		glDrawArrays(GL_POINTS, 0, inter_vertices.count);

		for (int i = 1; i < inter_flag.size(); i++)
			glDrawArrays(GL_LINE_STRIP, inter_flag[i - 1], inter_flag[i] - inter_flag[i - 1]);
		if (inter_flag.back() < inter_vertices.count - 1)
			glDrawArrays(GL_LINE_STRIP, inter_flag.back(), inter_vertices.count - inter_flag.back());


		glBindVertexArray(0);

		glfwSetMouseButtonCallback(window, mouse_button_callback);

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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		current_state = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		current_state = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		auto poly_inter_res = polygon_intersect_points(polygon1, polygon2);
		std::cout << "asdf " << poly_inter_res.size() << std::endl;
		for (auto item: poly_inter_res) {
			auto [x, y] = item;
			std::cout << x << ' ' << y << std::endl;
			inter_vertices.add_vertices(x, y, 0);
		}

		glBindVertexArray(VAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, inter_vertices.size(), inter_vertices.get_buffer_address(), GL_STATIC_DRAW);
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (not algo_flag) {
			Weiler_Atherton algo;
			auto res = algo.weiler_atherton_algorithm(polygon2, polygon1);
			res.print_polygon_message();

			for (auto loop: res.polygon->loops) {
				auto start = loop.start;
				auto cur = start;
				
				while(true) {
					inter_vertices.add_vertices(cur->vertex->x, cur->vertex->y, 0);
					cur = cur->succ;
					if (cur == start) break;
				}

				inter_vertices.add_vertices(start->vertex->x, start->vertex->y, 0);
				inter_flag.push_back(inter_vertices.count);
			}

			glBindVertexArray(VAO[2]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, inter_vertices.size(), inter_vertices.get_buffer_address(), GL_STATIC_DRAW);

			algo_flag = true;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		is_inner_state = not is_inner_state;
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