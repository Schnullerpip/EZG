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
	Shape* hitpoint_repr1 = nullptr, *hitpoint_repr2 = nullptr;
	Node* hitpoint_node1, * hitpoint_node2, *last_hitpoint;
	//will be given to console as event triggers
	bool draw_kd_tree = false;
	bool increase_maxd = false;
	bool decrease_maxd = false;
	bool kd_set_complexity = false;
	bool rebuild_kdt = false;
public:
	Light_And_Shadow(Input_Handler* i, EventFeedback* fb);
	virtual void render(GLfloat deltaTime)override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Light_And_Shadow();
	static void Light_And_Shadow::draw_bbox(BoundingBox* boundingbox, Camera* cam, Shader* s);
};

