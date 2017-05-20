#include "Test_Scene.h"
#include "Cube.h"
#include "Plane.h"
#include "KD_Tree.h"


void Test_Scene::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

Test_Scene::Test_Scene()
{
}

// Vertex shader
const GLchar* vertexShaderSrc = R"glsl(
	#version 330 core

	layout (location = 0) in float p;

	out float positionFeedback;

	void main()
	{
		positionFeedback = 33;
	}
)glsl";


void Test_Scene::render(GLfloat deltaTime)
{
    GLchar* feedbackVaryings[] = { "positionFeedback" };
	Shader* shad = new Shader("src/Shaders/SPG/Particles/updateParticle.vs", "src/Shaders/SPG/Particles/updateParticle.gs");
	shad->Link(feedbackVaryings, 1);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create input VBO and vertex format
    GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f , 6.0f};

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Create transform feedback buffer
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);

    // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

	shad->Use();
    glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, 2);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    glFlush();

    // Fetch and print results
    GLfloat feedback[6];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

    printf("%f %f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4], feedback[5]);

    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);
}


void Test_Scene::update(GLfloat deltaTime, EventFeedback* feedback)
{
}

Test_Scene::~Test_Scene()
{
}