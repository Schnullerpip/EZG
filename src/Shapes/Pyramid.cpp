#include "Pyramid.h"


GLuint Pyramid::VAO = 0;

Pyramid::Pyramid(glm::vec3 pos_glo, GLfloat width, GLfloat height, GLfloat depth, glm::vec3 pos_loc) : Shape()
{
	position_global	 = pos_glo;
	position_local	 = pos_loc;
	if (VAO == 0)
		glGenVertexArrays(1, &VAO);

	GLfloat w, h, d, x, y, z;
	w = width / 2;
	h = height / 2;
	d = depth / 2;
	x = pos_loc.x;
	y = pos_loc.y;
	z = pos_loc.z;
	
	vertices = std::vector<GLfloat>{
		//front
		x,   y+h, z,
		x-w, y-h, z+d,
		x+w, y-h, z+d,
		//left
		x,   y+h, z,
		x-w, y-h, z+d,
		x, y-h, z-d,
		//right
		x,   y+h, z,
		x+w, y-h, z+d,
		x, y-h, z-d,
		//bottom
		x-w, y-h, z+d,
		x, y-h, z-d,
		x+w, y-h, z+d,
	};
}

//void Pyramid::draw(glm::mat4 * model) {
//	if (model != nullptr && shader != nullptr) {
//		GLuint transformLoc = glGetUniformLocation(shader->Program, "transform");
//		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*model));
//	}
//	draw();
//}

void Pyramid::draw() {
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//copy data to the buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 36, &vertices[0], GL_STATIC_DRAW);
			//set vertex attribute pointers
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//draw
			glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Pyramid::~Pyramid()
{
}
