#pragma once
#include "Scene.h"
#include "KB_Interpolation.h"
#include "Cube.h"
#include "Squad.h"

class Kochanek_Bartels_Scene : public Scene {
	std::vector<glm::vec3> checkpoints;
	Camera cam;
	KB_Interpolation* kbi;
	Squad slerp;
	//Slerp slerp;

	Cube* currentPosition;
protected:
	virtual void init(int window_width, int window_height, const char* title = "")override;
public:
	Kochanek_Bartels_Scene();
	virtual void render(GLfloat deltaTime)override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Kochanek_Bartels_Scene();
};