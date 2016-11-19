#include "Cube.h"


Cube::Cube(Shader* s, glm::vec3 pos_glo, GLfloat width, GLfloat height, GLfloat depth, glm::vec3 pos_loc) : Shape() {

	shader = s;
	position_global = pos_glo;
	position_local = pos_loc;
	glGenVertexArrays(1, &VAO);

	GLfloat w, h, d, x, y, z;
	w = width / 2;
	h = height / 2;
	d = depth / 2;
	x = pos_loc.x;
	y = pos_loc.y;
	z = pos_loc.z;


	GLfloat left = x - w, right = x + w, up = y + h, down = y - h, front = z + d, back = z - d;
	
	vertices = std::vector<GLfloat> {
		//position			//texCoords   //normals
		//front
		left, down, front,  0.0f, 0.0f,   0.f, 0.f, 1.f,
		right, down, front, left, 0.0f,   0.f, 0.f, 1.f,
		right, up, front,	left, up,   0.f, 0.f, 1.f,

		right, up, front,	right, up,   0.f, 0.f, 1.f,
		left, up, front,	0.0f, up,   0.f, 0.f, 1.f,
		left, down, front,  0.0f, 0.0f,   0.f, 0.f, 1.f,

		//left
		left, up, back,		0.0f, up,   -1.f, 0.f, 0.f, 
		left, up, front,	front, up,   -1.f, 0.f, 0.f, 
		left, down, back,	0.0f, 0.0f, -1.f, 0.f, 0.f, 

		left, down, back,	0.0f, 0.0f,   -1.f, 0.f, 0.f, 
		left, down, front,  front, 0.0f,   -1.f, 0.f, 0.f, 
		left, up, front,	front, up,   -1.f, 0.f, 0.f, 

		//right
		right, up, front,	0.0f, up,   1.f, 0.f, 0.f,
		right, up, back,	back, up,   1.f, 0.f, 0.f,
		right, down, back,  back, 0.0f,   1.f, 0.f, 0.f,

		right, down, back,  back, 0.0f,   1.f, 0.f, 0.f,
		right, down, front, 0.0f, 0.0f,   1.f, 0.f, 0.f,
		right, up, front,	0.0f, up,   1.f, 0.f, 0.f,

		//top
		left, up, back,		0.0f, back,   0.f, 1.f, 0.f, 
		right, up, back,	right, back,   0.f, 1.f, 0.f,
		right, up, front,	right, 0.0f,   0.f, 1.f, 0.f,

		right, up, front,	right, 0.0f,   0.f, 1.f, 0.f,
		left, up, front,	0.0f, 0.0f,   0.f, 1.f, 0.f,
		left, up, back,		0.0f, back,   0.f, 1.f, 0.f,

		//bottom
		left, down, back,	0.0f, back,   0.f, -1.f, 0.f, 
		right, down, back,  right, back,   0.f, -1.f, 0.f, 
		right, down, front, right, 0.0f,   0.f, -1.f, 0.f, 

		right, down, front, right, 0.0f,   0.f, -1.f, 0.f, 
		left, down, front,  0.0f, 0.0f,   0.f, -1.f, 0.f, 
		left, down, back,	0.0f, back,   0.f, -1.f, 0.f, 

		//back
		left, down, back,	0.0f, up,   0.0f, 0.f, -1.f,
		right, down, back,  right, up,   0.0f, 0.f, -1.f,
		right, up, back,	right, 0.0f,   0.0f, 0.f, -1.f,

		right, up, back,	right, 0.0f,   0.0f, 0.f, -1.f,
		left, up, back,		0.0f, 0.0f,   0.0f, 0.f, -1.f,
		left, down, back,	0.0f, up,   0.0f, 0.f, -1.f
	};
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//copy data to the buffer
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
			//set vertex attribute pointers
			//position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*3));
			glEnableVertexAttribArray(1);
			//normals
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*5));
			glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Cube::draw()
{
	glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

Cube::~Cube()
{
}
