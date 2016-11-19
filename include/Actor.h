#pragma once
#include <GL/glew.h>

class Actor {
protected:
	bool active = true;
public:
	virtual void update(GLfloat deltaTime) = 0;
	virtual void render() = 0;
};
