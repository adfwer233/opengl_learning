#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <iostream>

#include "common/camera/camera.hxx"
#include "common/constructor/constructor.hxx"
#include "common/mesh_model.hxx"
#include "common/shader.hxx"
#include "common/skybox/skybox.h"
#include "common/simulation/solid_entity.hxx"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader"
#endif

#ifndef SKYBOX_DIR
#define SKYBOX_DIR "./skybox"
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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

	Shader shader(std::format("{}/lighting.vs", SHADER_DIR), std::format("{}/lighting.fs", SHADER_DIR));
	Shader shadow_map_shader(std::format("{}/shadow.vs", SHADER_DIR), std::format("{}/shadow.fs", SHADER_DIR));
	Shader debug_shadow_shader(std::format("{}/debug_shadow.vs", SHADER_DIR), std::format("{}/debug_shadow.fs", SHADER_DIR));
	Shader screenShader(std::format("{}/filter.vs", SHADER_DIR), std::format("{}/filter.fs", SHADER_DIR));

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


	float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	std::cout << sizeof(quad_vertices) << std::endl;

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	// set light position
	glm::vec3 lightPos(-7, 7, -10);

	MeshModel sphere = Constructor::Sphere(Point3d(1.5, 0, 0), 0.2);
	MeshModel sphere2 = Constructor::Sphere(Point3d(-0.5, -0.5, 1), 0.2);
	MeshModel light_src = Constructor::Sphere(Point3d(lightPos.x, lightPos.y, lightPos.z), 0.1);

	unsigned int VBO[10], VAO[10], EBO[10];

	const int ent_num = 5;

	glGenVertexArrays(ent_num, VAO);
	glGenBuffers(ent_num, VBO);
	glGenBuffers(ent_num, EBO);

	auto bind_mesh = [&](int index, MeshModel& model) {
		glBindVertexArray(VAO[index]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);

		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Point3d) * 2, model.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces_indices.size() * sizeof(TriangleVerticeIndex), model.faces_indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		};

	auto processMeshModelShadow = [&](int index, MeshModel& model, Shader& shader) {
		shader.use();
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model.transform));
		glBindVertexArray(VAO[index]);
		glDrawElements(GL_TRIANGLES, model.faces_indices.size() * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		};

	auto processMeshModel = [&](int index, MeshModel& model, Shader& shader, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.5f)) {
		glm::mat4 transform = glm::mat4(1.0f);
		//        transform = glm::rotate(model.transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

		projection = glm::perspective(glm::radians(camera.zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		shader.use();
		shader.set_vec3("objectColor", color);
		shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.set_vec3("lightPos", lightPos);
		shader.set_vec3("viewPos", camera.position);

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model.transform));

		unsigned int viewTransformLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewTransformLoc, 1, GL_FALSE, glm::value_ptr(camera.get_view_transformation()));

		unsigned int projectionTransformLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionTransformLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO[index]);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, model.faces_indices.size() * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		};

	auto cubic = Constructor::Cubic({ -0.3, -0.3, -0.3 }, { 0.3, 0.3, 0.3 });
	auto cubic2 = Constructor::Cubic({ -1.6, -0.3, -1.6 }, { -1.3, 0.3, -1.3 });

	bind_mesh(0, sphere);
	bind_mesh(1, sphere2);
	bind_mesh(2, cubic);
	bind_mesh(3, cubic2);
	bind_mesh(4, light_src);

	debug_shadow_shader.use();
	debug_shadow_shader.set_int("depthMap", 0);

	std::cout << "Construct finish " << std::endl;


	// sky box initialization
	std::vector<std::string> faces
	{
		std::format("{}/right.jpg", SKYBOX_DIR),
		std::format("{}/left.jpg", SKYBOX_DIR),
		std::format("{}/top.jpg", SKYBOX_DIR),
		std::format("{}/bottom.jpg", SKYBOX_DIR),
		std::format("{}/front.jpg", SKYBOX_DIR),
		std::format("{}/back.jpg", SKYBOX_DIR)
	};

	SkyBox skybox;
	skybox.load_cube_map(faces);
	skybox.bind();

	Shader skybox_shader(std::format("{}/simple.vs", SKYBOX_DIR), std::format("{}/simple.fs", SKYBOX_DIR));
	skybox_shader.use();
	skybox_shader.set_int("skybox", 0);

    screenShader.use();
    screenShader.set_int("screenTexture", 0);

	// simulation initialization

	auto field_function = [](glm::vec3 pos) -> glm::vec3 {
        // if (pos.z > 0)
		//     return glm::normalize(glm::vec3(0, 0, -pos.z));
        // else 
        auto len = glm::length(pos);
		if (len > 0.1)
        	return glm::normalize(-pos) / (len * len);
		else 
			return pos;
	};

	VectorField field(field_function);

	SolidEntity cubic_entity;
	cubic_entity.model = std::optional<std::reference_wrapper<MeshModel>>{ cubic };

	SolidEntity cubic_entity2;
	cubic_entity2.model = std::optional<std::reference_wrapper<MeshModel>>{ cubic2 };
	
	SolidEntity sphere_entity;
	sphere_entity.model = std::optional<std::reference_wrapper<MeshModel>>{ sphere };
    sphere_entity.velocity = glm::vec3(0, 0, -2);

	SolidEntity sphere_entity2;
	sphere_entity2.model = std::optional<std::reference_wrapper<MeshModel>>{ sphere2 };
    sphere_entity2.velocity = glm::vec3(0, 1, -2);

    std::vector<std::reference_wrapper<SolidEntity>> models = {
		std::reference_wrapper{sphere_entity},
		std::reference_wrapper{sphere_entity2},
		std::reference_wrapper{cubic_entity},
		std::reference_wrapper{cubic_entity2}
	};

	while (not glfwWindowShouldClose(window)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		sphere_entity.update_in_vector_field(field, deltaTime);
        sphere_entity2.update_in_vector_field(field, deltaTime);

		for (size_t i = 0; i < models.size(); i++) {
			for (size_t j = i + 1; j < models.size(); j++) {
				bool res = MeshModel::collision_test(models[i].get().model.value().get(), models[j].get().model.value().get());
				if (res) {
					if (i == 2 && j == 3) continue;
					std::cout << i << ' ' << j << std::endl;
					models[i].get().velocity = -models[i].get().velocity;
					models[j].get().velocity = -models[j].get().velocity;
				}
			}
		}

		processInput(window);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 100.5f;
		lightProjection = glm::perspective(glm::radians(camera.zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		shadow_map_shader.use();
		shadow_map_shader.set_mat4("lightSpaceMatrix", lightSpaceMatrix);

		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1, 0.1, 0.1, 1);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// step1: render the shadow map


		glClear(GL_DEPTH_BUFFER_BIT);
		processMeshModelShadow(0, sphere, shadow_map_shader);
		processMeshModelShadow(1, sphere2, shadow_map_shader);
		processMeshModelShadow(2, cubic, shadow_map_shader);
		processMeshModelShadow(3, cubic2, shadow_map_shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// step2: render the scene

		shader.use();
		shader.set_mat4("lightSpaceMatrix", lightSpaceMatrix);

		if (enable_filter)
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, depth_map);

		processMeshModel(0, sphere, shader);
		processMeshModel(1, sphere2, shader);
		processMeshModel(2, cubic, shader);
		processMeshModel(3, cubic2, shader);
		processMeshModel(4, light_src, shader, glm::vec3(10, 10, 10));

		glDepthMask(GL_LEQUAL);
		skybox_shader.use();
		unsigned int transformLoc = glGetUniformLocation(skybox_shader.ID, "model");
		auto scale = glm::mat4(1.0f);
		scale = glm::scale(scale, glm::vec3(10.0f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(scale));
		unsigned int viewTransformLoc = glGetUniformLocation(skybox_shader.ID, "view");
		glUniformMatrix4fv(viewTransformLoc, 1, GL_FALSE, glm::value_ptr(camera.get_view_transformation()));
		unsigned int projectionTransformLoc = glGetUniformLocation(skybox_shader.ID, "projection");
		glUniformMatrix4fv(projectionTransformLoc, 1, GL_FALSE, glm::value_ptr(projection));
		skybox.rendering();
		glDepthMask(GL_LESS);

		if (enable_filter) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
			glClear(GL_COLOR_BUFFER_BIT);

			screenShader.use();  
			glBindVertexArray(quadVAO);
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
