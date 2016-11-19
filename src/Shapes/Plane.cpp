#include "Plane.h"


GLuint Plane::VAO = 0;
GLuint Plane::EBO = 0;

Plane::Plane(glm::vec3 pos_glo, GLfloat width, GLfloat height, GLfloat depth, glm::vec3 pos_loc):Shape() {
	position_global = pos_glo;
	position_local = pos_loc;
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);
	}

	GLfloat w, h, d, x, y, z;
	w = width / 2;
	h = height/ 2;
	d = depth / 2;
	x = pos_loc.x;
	y = pos_loc.y;
	z = pos_loc.z;
	
	vertices = std::vector<GLfloat>{
		//up plane
		x - w, y + h, z + d, 0.0f, 0.0f,//0 
		x - w, y + h, z - d, 1.0f, 0.0f,//1
		x + w, y + h, z + d, 1.0f, 1.0f,//2
		x + w, y + h, z - d, 0.0f, 1.0f,//3
		//bottom plane
		x - w, y - h, z + d, 0.0f, 0.0f,//4
		x - w, y - h, z - d, 1.0f, 0.0f,//5
		x + w, y - h, z + d, 1.0f, 1.0f,//6
		x + w, y - h, z - d, 0.0f, 1.0f,//7
	};

	indices = new GLuint[36]{
		0,1,2,
		2,3,0,

		4,5,6,
		6,7,4,

		2,6,7,
		7,3,2,

		0,4,5,
		5,1,0,

		0,4,6,
		6,2,0,

		1,5,7,
		7,3,1
	};
}
///*void Plane::draw(glm::mat4 * model)
//{
//	if (model != nullptr && shader != nullptr) {
//		GLuint transformLoc = glGetUniformLocation(shader->Program, "transform");
//		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*model));
//	}
//	*/draw();
//}

void Plane::draw()
{
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

			//copy data to the buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*40, &vertices[0], GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*36, indices, GL_STATIC_DRAW);

			//set vertex attribute pointers
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			//draw
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



Plane::~Plane()
{
	delete[] indices;
	glDeleteBuffers(1, &EBO);
}
