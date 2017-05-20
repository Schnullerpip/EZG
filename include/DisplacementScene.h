#pragma once
#include "Scene.h"
#include "OnScreenConsole.h"

class DisplacementScene : public Scene
{
public:
	Input_Handler* input;
	OnScreenConsole* console;
	DisplacementScene(Input_Handler* ih, EventFeedback* ef);

	void render(GLfloat deltaTime) override;
	void update(GLfloat deltaTime, EventFeedback* feedback) override;

	~DisplacementScene();

	GLuint quadVAO = 0;
	GLuint quadVBO;

	GLboolean parallax_mapping = true;
	GLfloat height_scale = 0.1;

	glm::vec3 lightPos;

	//control variables 
	bool lightFollow = false, adjustCameraSpeed = false, toggleParallax = false, adjustNumLayers = false, adjustRefinement = false;
	GLfloat cameraSpeed = 20.f;
	GLfloat numLayers = 80.f;
	GLfloat numLayersRefinement = 40.f;
};