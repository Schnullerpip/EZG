#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"
#include "Texture.h"

class Shape
{
protected:
	GLuint VBO;
	std::vector<GLfloat> vertices;
public:
	GLuint VAO;
	std::vector<GLfloat> *getVertices();
	glm::vec3 position_global;
	glm::vec4 rotation;
	glm::vec4 scalation;

	glm::vec3 position_local;
	Shader* shader = nullptr;
	Texture* texture = nullptr;
	Texture* normalMap = nullptr;
	Shape() {
		glGenBuffers(1, &VBO);
	}
	inline glm::vec3 getPosition() const { return position_global; }
	inline glm::vec3 getPositionLocal() const { return position_local; }
	virtual void draw() = 0;
	virtual ~Shape() {
		//if (shader)delete shader; NOT SHAPES RESPONSIBLITY!!!!
		//if (texture)delete texture; NOT SHAPES RESPONSIBILITY!!!!
		glDeleteBuffers(1, &VBO);
	}
	static void drawLine(glm::vec3 from, glm::vec3 to, Shader*);
};

inline std::vector<GLfloat>* Shape::getVertices()
{
	return &vertices;
}

inline void Shape::drawLine(glm::vec3 from, glm::vec3 to, Shader* s)
{
	GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

	const GLfloat g_vertex_buffer_data[] = {
		from[0], from[1], from[2],
		to[0], to[1], to[2]
    };

	GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	s->Use();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
	glDrawArrays(GL_LINES, 0, 2); // 2 indices for the 2 end points of 1 line

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
}
