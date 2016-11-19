#pragma once
#include "Scene.h"
class Test_Scene : public Scene {
protected:
	virtual void init(int window_width, int window_height, const char* title) override;
public:
	Test_Scene();
	virtual void render()override;
	virtual void update(GLfloat deltaTime)override;
	~Test_Scene();
};

