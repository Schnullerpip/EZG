#include "SimpleCube.h"



SimpleCube::SimpleCube() : SimpleCube(new Shader())
{
}

SimpleCube::SimpleCube(Shader* s, glm::vec3 pos_glo, GLfloat width, GLfloat height, GLfloat depth, glm::vec3 pos_loc)
{
	shader = s;
	position_global = pos_glo;
	glGenVertexArrays(1, &VAO);

	float w = width/2, h = height/2, d=depth/2;

	std::vector<GLfloat> vec = {
	  //x		y		z
		//front
		-w,		h,		-d, //top left
		 w,		h,		-d, //top right
		-w,		-h,		-d, //bottom left
		 w,		-h,		-d, //bottom right
		//back
		-w,		h,		d, //top left
		 w,		h,		d, //top right
		-w,		-h,		d, //bottom left
		 w,		-h,		d, //bottom right
	};

	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vec.size()*sizeof(GLfloat), &vec[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//the order of how the cube is drawn
	GLushort elements[] = {
	  0, 1, 2, 3,
	  4, 5, 6, 7,
	  0, 4, 1, 5, 2, 6, 3, 7
	};
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SimpleCube::~SimpleCube()
{
}

void SimpleCube::draw()
{
	//glBindVertexArray(VAO);
	//	glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
