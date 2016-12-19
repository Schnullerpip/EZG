#pragma once
#include <vector>
#include "Shader.h"
#include "Shape.h"
#include "Camera.h"
#include "EventFeedback.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include "Light.h"

class Scene {
protected:
	Camera cam;
	std::vector<Shader*> shader;
	std::vector<Shape*> shape;
	std::vector<Light*> light;
	std::vector<Texture*> texture;
	EventFeedback* feedback;
	virtual void init(int window_width, int window_height, const char* title = "");
public:
	int width, height;
	GLFWwindow* window;
	virtual void render(GLfloat deltaTime) = 0;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback) = 0;
};

inline void Scene::init(int window_width, int window_height, const char* title) {
	//if something should be initialized for all the scenes do it here!
} 