#pragma once
#include <GL/glew.h>
#include "Shape.h"
#include "Actor.h"
#include <GLFW/glfw3.h>

class Light : public Actor
{
protected:
	Shape* object;
	Shader* shader;
	Shader* depthShader;
	Shader* depthShaderVSM;

	glm::vec3 lightColor; 

	//for shadow
	GLuint	SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO, depthMapFBOVSM, depthCubeMap, depthCubeMapVSM;
	GLfloat far = 800.f;
public:
	void setShader(Shader* s) { shader = s; }
	void setTexture(Texture* t) const { object->texture = t; }
	void setLightColor(glm::vec3 l) { lightColor = l; }
	void setPosition(glm::vec3 p) const { object->position_global = p; }

	void apply_to(Shader* s) const;
	void apply_shadow_to(Shader* s)const;

	Shader* getShader()const { return shader; }
	glm::vec3 getPosition()const { return object->getPosition(); }
	glm::vec3 getLightColor() const { return lightColor; }
	Shape* getObject()const { return object; }
	GLuint getDepthCubeMap()const { return depthCubeMap; }
	GLuint getDepthCubeMapVSM()const { return depthCubeMapVSM; }
	Shader* renderShadow(std::vector<Shape*> shapes, GLFWwindow* window);
	GLuint renderShadowVSM(std::vector<Shape*> shapes, GLFWwindow* window);

	void update(GLfloat deltaTime)override;
	void render()override;
	Light(Shader* s, glm::vec3 position, GLfloat width = 1.f, GLfloat height = 1.f, GLfloat depth = 1.f);
	~Light();
};

