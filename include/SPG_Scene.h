#pragma once
#include "Scene.h"
#include "OnScreenConsole.h"
#include "KD_Tree.h"
#include <random>

const int particle_num = 10000;
const int particle_elements = 3 * 2; //x,y,z, type, life

struct marching_geo : public Shape
{
	void draw() override {}

	void addVertices(GLfloat* begin, GLfloat* end)
	{
		vertices.insert(vertices.end(), begin, end);
	}
};

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
	Shader *renderParticle;

	marching_geo *geometry;
	KD_Tree* kdt;

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
	const size_t INPUT = 0, FEEDBACK_P = 1, FEEDBACK_T = 2, FEEDBACK_L = 3;
	GLuint particle_vao, particle_vbo[4];
	GLfloat particle_vertices[particle_num*particle_elements];
	std::mt19937 generator;
	std::uniform_real_distribution<float> probability;
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