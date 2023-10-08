#include "common/shader.hxx"
#include <sstream>

Shader::Shader(const std::string vertex_path, const std::string fragment_path) {
	std::string vertex_shader_source, fragment_shader_source;
	std::ifstream vertex_ifstream, fragment_ifstream;

	auto read_source = [](std::string& target, const std::string& path) {
		std::ifstream input_stream;
		std::stringstream input_string_stream;
		input_stream.open(path);
		input_string_stream << input_stream.rdbuf();
		target = input_string_stream.str();
		};

	read_source(vertex_shader_source, vertex_path);
	read_source(fragment_shader_source, fragment_path);

	unsigned int vertex, fragment;

	// @TODO: check the result of compilation and linking.

	auto vertex_code = vertex_shader_source.c_str();
	auto fragment_code = fragment_shader_source.c_str();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, nullptr);
	glCompileShader(vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, nullptr);
	glCompileShader(fragment);

	this->ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	glLinkProgram(ID);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}