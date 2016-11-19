#include "Triangle.h"

GLuint Triangle::VAO = 0;

Triangle::Triangle(vec3 P0, vec3 P1, vec3 P2) : Shape()
{
	vertices[0] = P0.x;
	vertices[1] = P0.y;
	vertices[2] = P0.z;

	vertices[3] = P1.x;
	vertices[4] = P1.y;
	vertices[5] = P1.z;

	vertices[6] = P2.x;
	vertices[7] = P2.y;
	vertices[8] = P2.z;
	
	if (VAO == 0)
		glGenVertexArrays(1, &VAO);

}

Triangle::~Triangle()
{
	glDeleteVertexArrays(1, &VAO);
}

//void Triangle::draw(glm::mat4* model)
//{
//	if (model != nullptr && shader != nullptr) {
//		GLuint transformLoc = glGetUniformLocation(shader->Program, "transform");
//		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*model));
//	}
//	draw();
//}

void Triangle::draw()
{
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//copy data to the buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			//set vertex attribute pointers
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//draw
			glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
