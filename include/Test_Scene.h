#pragma once
#include "Scene.h"
class Test_Scene : public Scene {
protected:
	virtual void init(int window_width, int window_height, const char* title) override;
public:
	GLuint quadVAO = 0;
	GLuint quadVBO;

	Texture* texture;
	Texture* normalMap;

	Shader* shader;

	glm::vec3 lightPos;

	Test_Scene();
	virtual void render()override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Test_Scene();
};

