#include "Test_Scene.h"
#include "Cube.h"
#include "Plane.h"


void Test_Scene::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

Test_Scene::Test_Scene()
{
	shader.push_back(new Shader("src/Shaders/simpleVertexShader.vs", "src/Shaders/simpleFragmentShader.fs", "src/Shaders/pointToHouse.gs"));
}

void Test_Scene::render()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GLfloat points[] = {
	-0.5f,  0.5f, 0.f, // Top-left
	 0.5f,  0.5f, 0.f, // Top-right
	 0.5f, -0.5f, 0.f, // Bottom-right
	-0.5f, -0.5f, 0.f // Bottom-left
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);



	shader[0]->Use();
	glDrawArrays(GL_POINTS, 0, 4); 
	glBindVertexArray(0);
}

void Test_Scene::update(GLfloat deltaTime)
{
}

Test_Scene::~Test_Scene()
{
}
