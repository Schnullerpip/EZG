#pragma once
#include "Scene.h"
#include "Input_Handler.h"
#include "OnScreenConsole.h"
#include "KD_Tree.h"
#include "Cube.h"

class Light_And_Shadow : public Scene
{
protected:
	void init(int window_width, int window_height, const char* title = "")override;
	Input_Handler* input;
	OnScreenConsole* console;
	KD_Tree* kdt;
	std::vector<Cube*> boundingBoxRepresentation;
public:
	Light_And_Shadow(Input_Handler* i, EventFeedback* fb);
	virtual void render(GLfloat deltaTime)override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Light_And_Shadow();
	static void Light_And_Shadow::draw_bbox(BoundingBox* boundingbox, Camera* cam, Shader* s);
};

