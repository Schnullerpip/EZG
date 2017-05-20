#pragma once
#include "Scene.h"
#include "OnScreenConsole.h"

const int particle_num = 10;

class SPG_Scene : public Scene
{
	Input_Handler* input;
	OnScreenConsole* console;
	bool renderTextureDebug = false;

	Shader *densityShader;
	Shader *test;
	Shader *createGeometry;
	Shader *renderGeometry;
	Shader *updateParticle;

	GLfloat points[30] = {
		//coord			//tex coord
		-.5f, -.5f,0,	0.f, 0.f,
		.5f,  -.5f,0,	1.f, 0.f,
		-.5f,  .5f,0,	0.f, 1.f,
             
		.5f, -.5f, 0,	1.f, 0.f,
		.5f, .5f,  0,   1.f, 1.f,
		-.5f, .5f, 0,	0.f, 1.f,
	};

	GLuint VAO, VBO, textureVertexArray, textureVertexBuffer, polygonVAO[8], polygonVBO[8];

	//Marcing Cubes Specific
	GLchar** createGeometryFeedbackVaryings;
	GLuint tex_3d, texFrameBuffer, lutEdges;

	//particles specific
	const size_t INPUT = 0, FEEDBACK = 1;
	GLuint particle_vao, particle_vbo[2];
	GLfloat particle_vertices[particle_num*3];
public:
	void transferParticleBuffers() {}
	void setEmitter(GLfloat x, GLfloat y, GLfloat z) {}
	static const size_t xDim_texture3d = 96, yDim_texture3d = 96, zDim_texture3d = 256;
private:
	GLfloat texture3d[xDim_texture3d][yDim_texture3d][zDim_texture3d] = {};
	//rock pillars
	glm::vec2 pillar[3] = { glm::vec2(0.4f, -0.4f), glm::vec2(-0.6f, -0.2f) , glm::vec2(0.3, 0.4f)};

public:
	SPG_Scene(Input_Handler* ih, EventFeedback* ef);
	~SPG_Scene();
	void render(GLfloat deltaTime) override;
	void update(GLfloat deltaTime, EventFeedback* feedback) override;
};