#pragma once
#include "Scene.h"
#include "Input_Handler.h"
#include "OnScreenConsole.h"
#include "KD_Tree.h"
#include "Cube.h"
#include "KB_Interpolation.h"
#include "Squad.h"

class Light_And_Shadow : public Scene
{
protected:
	void init(int window_width, int window_height, const char* title = "")override;
	void renderKD_Node(Node* node, Shader* s, Camera* cam);
	void renderKD(Node* node, Shader* s, Camera* cam, int max_depth, Node* hit_node);
	KD_Tree* kdt, *paris_kdt;
	Input_Handler* input;
	OnScreenConsole* console;
	Shape* hitpoint_repr1 = nullptr, *hitpoint_repr2 = nullptr, *bbox_representation = nullptr;
	Node* hitpoint_node1, * hitpoint_node2, *last_hitpoint;

	KB_Interpolation* kb;
	Squad squad;

	//will be given to console as event triggers
	std::vector<Shape*> loadable_shapes_dog;
	std::string dog_arguments, paris_arguments;

	bool kbi = false;
	bool ray_paris = false;
	bool draw_kd_tree = false;
	bool adjust_maxd = false;
	bool kd_set_complexity = false;
	bool rebuild_kdt = false;

	void rebuildKDT();
public:
	Light_And_Shadow(Input_Handler* i, EventFeedback* fb);
	void render(GLfloat deltaTime)override;
	void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Light_And_Shadow();
	static void Light_And_Shadow::draw_bbox(BoundingBox* boundingbox, Camera* cam, Shader* s);
};

