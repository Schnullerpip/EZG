#include "Test_Scene.h"
#include "Cube.h"
#include "Plane.h"
#include "KD_Tree.h"


void Test_Scene::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

Test_Scene::Test_Scene()
{
}

void Test_Scene::render(GLfloat deltaTime) {}


void Test_Scene::update(GLfloat deltaTime, EventFeedback* feedback)
{
}

Test_Scene::~Test_Scene()
{
}