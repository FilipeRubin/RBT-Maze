#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>
#include <stack>
#include <iostream>

#define WALL_UP    0x01
#define WALL_DOWN  0x02
#define WALL_LEFT  0x04
#define WALL_RIGHT 0x08

const int MAZE_WIDTH = 20;
const int MAZE_HEIGHT = 15;

typedef unsigned char byte;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

class Cube3D
{
private:
	static GLuint s_vbo;
	static GLuint s_vao;
	static GLuint s_ebo;
	static GLuint s_diffuseTexture;
	static GLuint s_specularTexture;
	static GLuint s_shaderProgram;
	static GLint s_projUniform;
	static GLint s_viewUniform;
	static GLint s_worldUniform;
	static GLint s_diffuseUniform;
	static GLint s_specularUniform;
	static glm::mat4 s_worldMat;

	struct Vertex
	{
		float  x,  y,  z;
		float nx, ny, nz;
		float  u,  v;
	};

	struct Color
	{
		byte r, g, b;
	};
public:
	static void Init()
	{
		const Vertex cube[]
		{
			// Front face
			{  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f },
			{  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f },
			{ -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f },
			// Right face
			{  1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f },
			{  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f },
			{  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
			// Back face
			{ -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f },
			{ -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f },
			{  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f },
			{  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f },
			// Left face
			{ -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f },
			{ -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f },
			{ -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f },
			// Top face
			{  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f },
			{  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f },
			{ -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f },
			{ -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },
			// Bottom face
			{  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f },
			{  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f },
			{ -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f }
		};

		const GLubyte indices[]
		{
			0, 1, 2,
			0, 2, 3,
			4, 5, 6,
			4, 6, 7,
			8, 9, 10,
			8, 10, 11,
			12, 13, 14,
			12, 14, 15,
			16, 17, 18,
			16, 18, 19,
			20, 21, 22,
			20, 22, 23
		};

		constexpr Color diffusePixels[]
		{
			{ 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 216,   0}, {255, 216,   0}, {255, 216,   0}, {255, 216,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {255, 216,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 216,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, {255, 106,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, { 64,  64,  64},
			{ 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}, { 64,  64,  64}
		};

		constexpr Color specularPixels[]
		{
			{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {  0,   0,   0}, {255, 255, 255},
			{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}
		};

		glGenBuffers(1, &s_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
		
		glGenVertexArrays(1, &s_vao);
		glBindVertexArray(s_vao);
		glVertexAttribPointer(0U, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
		glVertexAttribPointer(1U, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
		glVertexAttribPointer(2U, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
		glEnableVertexAttribArray(0U);
		glEnableVertexAttribArray(1U);
		glEnableVertexAttribArray(2U);

		glGenBuffers(1, &s_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glGenTextures(1, &s_diffuseTexture);
		glBindTexture(GL_TEXTURE_2D, s_diffuseTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, diffusePixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		glGenTextures(1, &s_specularTexture);
		glBindTexture(GL_TEXTURE_2D, s_specularTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, specularPixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		const GLchar* vs_source = R"(
#version 330 core

layout(location = 0) in vec3 v_in_pos;
layout(location = 1) in vec3 v_in_nor;
layout(location = 2) in vec2 v_in_uv;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_world;

out vec3 v_out_pos;
out vec3 v_out_nor;
out vec2 v_out_uv;

void main()
{
    gl_Position = u_projection * u_view * u_world * vec4(v_in_pos, 1.0);
    v_out_pos = vec3(u_world * vec4(v_in_pos, 1.0));
    v_out_nor = mat3(transpose(inverse(u_world))) * v_in_nor;
    v_out_uv  = v_in_uv;
}
)";
		
		const GLchar* fs_source = R"(
#version 330 core

in vec3 v_out_pos;
in vec3 v_out_nor;
in vec2 v_out_uv;

out vec4 f_color;

uniform sampler2D u_diffuse;
uniform sampler2D u_specular;

void main()
{
    vec3 lightDir = normalize(vec3(0.3, 0.3, 1.0));

    // Texture
    vec3 diffColor = vec3(texture(u_diffuse, v_out_uv));
    vec3 specColor = vec3(texture(u_specular, v_out_uv));

    // Ambient lighting
    float ambient = 0.15;

    // Diffuse lighting
    float diffuse = max(dot(v_out_nor, lightDir), 0.0);

    // Specular highlight
    vec3 viewDir = normalize(-v_out_pos);
    vec3 reflectDir = reflect(-lightDir, v_out_nor);
	vec3 specular = vec3(0.9) * pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
	f_color = vec4((ambient + diffuse) * diffColor + (specular * specColor), 1.0);
}
)";

		s_shaderProgram = glCreateProgram();
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vs, 1, &vs_source, 0);
		glShaderSource(fs, 1, &fs_source, 0);
		glCompileShader(vs);
		glCompileShader(fs);
		glAttachShader(s_shaderProgram, vs);
		glAttachShader(s_shaderProgram, fs);
		glLinkProgram(s_shaderProgram);

		s_projUniform = glGetUniformLocation(s_shaderProgram, "u_projection");
		s_viewUniform =	glGetUniformLocation(s_shaderProgram, "u_view");
		s_worldUniform = glGetUniformLocation(s_shaderProgram, "u_world");
		s_diffuseUniform = glGetUniformLocation(s_shaderProgram, "u_diffuse");
		s_specularUniform = glGetUniformLocation(s_shaderProgram, "u_specular");

		glm::mat4 projMat = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 10.0f);
		glm::mat4 viewMat(1.0f);

		glUseProgram(s_shaderProgram);

		glUniformMatrix4fv(s_projUniform, 1, GL_FALSE, &projMat[0][0]);
		glUniformMatrix4fv(s_viewUniform, 1, GL_FALSE, &viewMat[0][0]);
		glUniform1i(s_diffuseUniform, 0);
		glUniform1i(s_specularUniform, 1);
	}

	static void Render()
	{
		static float angleY = 0.0f;
		static float angleX = 0.0f;
		glm::mat4 pos = glm::translate(glm::mat4(1.0f), glm::vec3(cosf((float)glfwGetTime()) * 2.0f, -1.5f * sinf((float)glfwGetTime()), 3.0f * sinf((float)glfwGetTime() * 0.3f) - 6.0f));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
		s_worldMat = pos * rotY * rotX;
		glUseProgram(s_shaderProgram);
		glUniformMatrix4fv(s_worldUniform, 1, GL_FALSE, &s_worldMat[0][0]);
		glBindVertexArray(s_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ebo);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, s_diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, s_specularTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)0);
		angleY += 0.01f;
		angleX += 0.003f;
	}

	static void Cleanup()
	{
		glDeleteBuffers(1, &s_vbo);
		glDeleteVertexArrays(1, &s_vao);
		glDeleteBuffers(1, &s_ebo);
		glDeleteTextures(1, &s_diffuseTexture);
		glDeleteTextures(1, &s_specularTexture);
		glDeleteProgram(s_shaderProgram);
	}
};

GLuint Cube3D::s_vbo = 0U;
GLuint Cube3D::s_vao = 0U;
GLuint Cube3D::s_ebo = 0U;
GLuint Cube3D::s_diffuseTexture = 0U;
GLuint Cube3D::s_specularTexture = 0U;
GLuint Cube3D::s_shaderProgram = 0U;
GLint Cube3D::s_projUniform = 0;
GLint Cube3D::s_viewUniform = 0;
GLint Cube3D::s_worldUniform = 0;
GLint Cube3D::s_diffuseUniform = 0;
GLint Cube3D::s_specularUniform = 0;
glm::mat4 Cube3D::s_worldMat = glm::mat4(1.0f);

class Shader
{
private:
	static GLuint s_vbo;
	static GLuint s_vao;
	static GLuint s_shaderProgram;
	static GLint s_posUniform;
	static GLint s_colUniform;
public:
	static void Init()
	{
		float data[8];

		data[0] = 1.0f / (float)MAZE_WIDTH;
		data[1] = 0.0f;

		data[2] = 1.0f / (float)MAZE_WIDTH;
		data[3] = 1.0f / (float)MAZE_HEIGHT;

		data[4] = 0.0f;
		data[5] = 1.0f / (float)MAZE_HEIGHT;

		data[6] = 0.0f;
		data[7] = 0.0f;

		glGenBuffers(1, &s_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

		glGenVertexArrays(1, &s_vao);
		glBindVertexArray(s_vao);
		glVertexAttribPointer(0U, 2, GL_FLOAT, GL_FALSE, 8, (void*)0);
		glEnableVertexAttribArray(0U);

		const GLchar* vs_source = R"(
#version 330 core

layout(location = 0) in vec2 v_pos;

uniform vec2 u_pos;

void main()
{
	gl_Position = vec4(((v_pos.x + u_pos.x) * 2.0) - 1.0, ((v_pos.y + u_pos.y) * 2.0) - 1.0, 0.0, 1.0);
}
)";
		const GLchar* fs_source = R"(
#version 330 core

uniform vec3 u_col;

out vec4 f_color;

void main()
{
	f_color = vec4(u_col.r, u_col.g, u_col.b, 1.0);
}
)";

		s_shaderProgram = glCreateProgram();
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vs, 1, &vs_source, 0);
		glShaderSource(fs, 1, &fs_source, 0);
		glCompileShader(vs);
		glCompileShader(fs);
		glAttachShader(s_shaderProgram, vs);
		glAttachShader(s_shaderProgram, fs);
		glLinkProgram(s_shaderProgram);
		glDeleteShader(vs);
		glDeleteShader(fs);

		s_posUniform = glGetUniformLocation(s_shaderProgram, "u_pos");
		s_colUniform = glGetUniformLocation(s_shaderProgram, "u_col");
	}

	static void Cleanup()
	{
		glDeleteBuffers(1, &s_vbo);
		glDeleteVertexArrays(1, &s_vao);
		glDeleteProgram(s_shaderProgram);
	}

	static void Use()
	{
		glUseProgram(s_shaderProgram);
	}

	static void BindCell()
	{
		glBindVertexArray(s_vao);
	}

	static GLint GetPosUniform()
	{
		return s_posUniform;
	}

	static GLint GetColUniform()
	{
		return s_colUniform;
	}
};

GLuint Shader::s_vbo = 0U;
GLuint Shader::s_vao = 0U;
GLuint Shader::s_shaderProgram = 0U;
GLint  Shader::s_posUniform = 0;
GLint  Shader::s_colUniform = 0;

class Cell
{
private:
	int m_x, m_y;
	float m_r, m_g, m_b;
	byte m_walls;
public:
	Cell(int x = 0, int y = 0)
		:m_x(x), m_y(y), m_r(1.0f), m_g(1.0f), m_b(1.0f), m_walls(0x0f) {}
	
	void GetUnitPosition(int& x, int& y) const
	{
		x = m_x;
		y = m_y;
	}

	void SetPosition(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

	void SetColor(float r, float g, float b)
	{
		m_r = r;
		m_g = g;
		m_b = b;
	}

	byte GetWalls() const
	{
		return m_walls;
	}

	void RemoveWalls(byte walls)
	{
		m_walls ^= walls;
	}

	void Render() const
	{
		glUniform2f(Shader::GetPosUniform(), (float)m_x / MAZE_WIDTH, (float)m_y / MAZE_HEIGHT);
		glUniform3f(Shader::GetColUniform(), m_r, m_g, m_b);
		Shader::BindCell();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glUniform3f(Shader::GetColUniform(), 0.0f, 0.0f, 0.0f);
		glDepthFunc(GL_ALWAYS);
		if (m_walls & WALL_UP) // Top wall
			glDrawArrays(GL_LINES, 1, 2);
		if (m_walls & WALL_DOWN) // Bottom wall
			glDrawArrays(GL_LINES, 3, 2);
		if (m_walls & WALL_LEFT) // Left wall
			glDrawArrays(GL_LINES, 2, 2);
		if (m_walls & WALL_RIGHT) // Right wall
			glDrawArrays(GL_LINES, 0, 2);
		glDepthFunc(GL_LESS);
	}
};

class RandomMazeGenerator
{
private:
	Cell(&m_maze)[MAZE_WIDTH][MAZE_HEIGHT];
	int m_visitedCount;
	bool m_visitedList[MAZE_WIDTH][MAZE_HEIGHT];
	std::stack<Cell*> m_path;
	Cell* m_pFirstCell;
	Cell* m_pLastCell;
	bool m_lastChecked;
public:
	RandomMazeGenerator(Cell(&maze)[MAZE_WIDTH][MAZE_HEIGHT])
		:m_maze(maze), m_visitedCount(0), m_path(std::stack<Cell*>()), m_pFirstCell(nullptr), m_pLastCell(nullptr), m_lastChecked(false)
	{
		memset(m_visitedList, false, sizeof(m_visitedList));
	}
	
	bool Step()
	{
		if (m_visitedCount == MAZE_HEIGHT * MAZE_WIDTH and m_path.empty())
		{
			return true;
		}
		if (m_path.empty())
		{
			int x, y;
			do
			{
				x = rand() % MAZE_WIDTH;
				y = rand() % MAZE_HEIGHT;
			} while (m_visitedList[x][y]);
			m_pFirstCell = &m_maze[x][y];
			m_visitedList[x][y] = true;
			m_visitedCount++;
			m_maze[x][y].SetColor(0.1f, 0.8f, 0.5f);
			m_path.push(&m_maze[x][y]);
		}
		else
		{
			byte nextDir = 0U;

			int x, y; // Position of last Cell in stack
			m_path.top()->GetUnitPosition(x, y);
			
			byte directionsChecked = 0U;

			do
			{
				byte rDir = 0U; // Random direction
				do
				{
					int sorted = rand() % 4;
					rDir = 0x01 << sorted;
				} while ((rDir & directionsChecked) != 0U);

				directionsChecked |= rDir;

				switch (rDir)
				{
				case WALL_UP:
					if (y + 1 < MAZE_HEIGHT and m_visitedList[x][y + 1] == false)
					{
						m_visitedList[x][y + 1] = true;
						m_visitedCount++;
						m_maze[x][y + 1].SetColor(0.1f, 0.8f, 0.5f);
						m_maze[x][y + 1].RemoveWalls(WALL_DOWN);
						m_maze[x][y].RemoveWalls(WALL_UP);
						m_path.push(&m_maze[x][y + 1]);
						nextDir = rDir;
					}
					break;
				case WALL_DOWN:
					if (y - 1 >= 0 and m_visitedList[x][y - 1] == false)
					{
						m_visitedList[x][y - 1] = true;
						m_visitedCount++;
						m_maze[x][y - 1].SetColor(0.1f, 0.8f, 0.5f);
						m_maze[x][y - 1].RemoveWalls(WALL_UP);
						m_maze[x][y].RemoveWalls(WALL_DOWN);
						m_path.push(&m_maze[x][y - 1]);
						nextDir = rDir;
					}
					break;
				case WALL_LEFT:
					if (x - 1 >= 0 and m_visitedList[x - 1][y] == false)
					{
						m_visitedList[x - 1][y] = true;
						m_visitedCount++;
						m_maze[x - 1][y].SetColor(0.1f, 0.8f, 0.5f);
						m_maze[x - 1][y].RemoveWalls(WALL_RIGHT);
						m_maze[x][y].RemoveWalls(WALL_LEFT);
						m_path.push(&m_maze[x - 1][y]);
						nextDir = rDir;
					}
					break;
				case WALL_RIGHT:
					if (x + 1 < MAZE_WIDTH and m_visitedList[x + 1][y] == false)
					{
						m_visitedList[x + 1][y] = true;
						m_visitedCount++;
						m_maze[x + 1][y].SetColor(0.1f, 0.8f, 0.5f);
						m_maze[x + 1][y].RemoveWalls(WALL_LEFT);
						m_maze[x][y].RemoveWalls(WALL_RIGHT);
						m_path.push(&m_maze[x + 1][y]);
						nextDir = rDir;
					}
					break;
				}
			} while (directionsChecked != 0x0f and nextDir == 0x00); // While still didn't check all directions AND there's no defined next direction

			if (nextDir == 0U) // There's nowhere to go
			{
				m_path.top()->SetColor(0.1f, 0.6f, 0.8f);
				m_path.pop();
			}
			if (m_visitedCount == MAZE_WIDTH * MAZE_HEIGHT and !m_lastChecked)
			{
				m_pLastCell = m_path.top();
				m_lastChecked = true;
			}
		}
		return false;
	}

	Cell* GetFirstCell()
	{
		return m_pFirstCell;
	}

	Cell* GetLastCell()
	{
		return m_pLastCell;
	}
};

class Player
{
private:
	const Cell(&m_maze)[MAZE_WIDTH][MAZE_HEIGHT];
	GLFWwindow* m_pWindow;
	GLuint m_vbo;
	GLuint m_vao;
	int m_x;
	int m_y;
	bool keyState[4];
public:
	Player(const Cell(&maze)[MAZE_WIDTH][MAZE_HEIGHT], GLFWwindow* pWindow)
		:m_maze(maze), m_pWindow(pWindow), m_vbo(0U), m_vao(0U), m_x(0), m_y(0)
	{
		memset(keyState, false, sizeof(keyState));
		float vertices[6];
		vertices[0] = 1.0f / MAZE_WIDTH * 0.9f;
		vertices[1] = 1.0f / MAZE_HEIGHT * 0.1f;

		vertices[2] = 1.0f / MAZE_WIDTH * 0.5f;
		vertices[3] = 1.0f / MAZE_HEIGHT * 0.9f;

		vertices[4] = 1.0f / MAZE_WIDTH * 0.1f;
		vertices[5] = 1.0f / MAZE_HEIGHT * 0.1f;

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glVertexAttribPointer(0U, 2, GL_FLOAT, GL_FALSE, 8, (void*)0);
		glEnableVertexAttribArray(0U);
	}

	void GetUnitPosition(int& x, int& y) const
	{
		x = m_x;
		y = m_y;
	}

	void SetUnitPosition(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

	void Process()
	{
		if (glfwGetKey(m_pWindow, GLFW_KEY_UP) == GLFW_PRESS and keyState[0] == false)
		{
			keyState[0] = true;
			if ((m_maze[m_x][m_y].GetWalls() & WALL_UP) == 0x00)
				m_y += 1;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS and keyState[1] == false)
		{
			keyState[1] = true;
			if ((m_maze[m_x][m_y].GetWalls() & WALL_DOWN) == 0x00)
				m_y -= 1;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS and keyState[2] == false)
		{
			keyState[2] = true;
			if ((m_maze[m_x][m_y].GetWalls() & WALL_LEFT) == 0x00)
				m_x -= 1;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS and keyState[3] == false)
		{
			keyState[3] = true;
			if ((m_maze[m_x][m_y].GetWalls() & WALL_RIGHT) == 0x00)
				m_x += 1;
		}

		if (glfwGetKey(m_pWindow, GLFW_KEY_UP) == GLFW_RELEASE)
		{
			keyState[0] = false;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_DOWN) == GLFW_RELEASE)
		{
			keyState[1] = false;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_LEFT) == GLFW_RELEASE)
		{
			keyState[2] = false;
		}
		if (glfwGetKey(m_pWindow, GLFW_KEY_RIGHT) == GLFW_RELEASE)
		{
			keyState[3] = false;
		}
	}

	void Render()
	{
		glDepthFunc(GL_ALWAYS);
		glBindVertexArray(m_vao);
		glUniform2f(Shader::GetPosUniform(), (float)m_x / MAZE_WIDTH, (float)m_y / MAZE_HEIGHT);
		glUniform3f(Shader::GetColUniform(), 0.9f, 0.25f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUniform3f(Shader::GetColUniform(), 0.0f, 0.0f, 0.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
		glDepthFunc(GL_LESS);
	}

	~Player()
	{
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}
};

int main()
{
	if (glfwInit() == GLFW_FALSE)
		return -1;

	GLFWwindow* pWindow = glfwCreateWindow(800, 600, "Maze by Rubin", nullptr, nullptr);
	if (pWindow == nullptr)
	{
		glfwTerminate();
		return -1;
	}

	srand(time(NULL));

	glfwSetWindowSizeCallback(pWindow, window_size_callback);
	glfwMakeContextCurrent(pWindow);
	glfwSwapInterval(1);

	if (!gladLoadGL())
	{
		glfwDestroyWindow(pWindow);
		glfwTerminate();
		return -1;
	}

	Cube3D::Init();
	Shader::Init();

	Cell maze[MAZE_WIDTH][MAZE_HEIGHT];

	RandomMazeGenerator rmg = RandomMazeGenerator(maze);

	for (int i = 0; i < MAZE_WIDTH; i++)
	{
		for (int j = 0; j < MAZE_HEIGHT; j++)
		{
			maze[i][j].SetPosition(i, j);
			if ((i + j) % 2 == 0)
				maze[i][j].SetColor(0.1f, 0.7f, 0.6f);
			else
				maze[i][j].SetColor(0.1f, 0.7f, 0.65f);
		}
	}

	Player player = Player(maze, pWindow);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (!glfwWindowShouldClose(pWindow))
	{
		Shader::Use();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < MAZE_WIDTH; i++)
		{
			for (int j = 0; j < MAZE_HEIGHT; j++)
			{
				maze[i][j].Render();
			}
		}
		{
			static int firstX = -1, firstY = -1;
			if (firstX == -1)
			{
				rmg.Step();
				rmg.GetFirstCell()->GetUnitPosition(firstX, firstY);
				player.SetUnitPosition(firstX, firstY);
			}
		}

		static Cell* pLastCell = nullptr;

		if (rmg.Step())
		{
			static bool initialized = false;
			if (!initialized)
			{
				pLastCell = rmg.GetLastCell();
				pLastCell->SetColor(1.0f, 0.9f, 0.75f);
				initialized = true;
			}

			player.Process();
			player.Render();
		}

		if (pLastCell != nullptr)
		{
			int lastX, lastY;
			pLastCell->GetUnitPosition(lastX, lastY);
			int playerX, playerY;
			player.GetUnitPosition(playerX, playerY);
			if (lastX == playerX and lastY == playerY)
			{
				glClear(GL_DEPTH_BUFFER_BIT);
				Cube3D::Render();
			}
		}

		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}
	
	Shader::Cleanup();
	Cube3D::Cleanup();
	glfwDestroyWindow(pWindow);
	glfwTerminate();
	return 0;
}