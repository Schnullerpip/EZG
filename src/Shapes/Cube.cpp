#include "Cube.h"
#include <vector>

Cube::Cube(Shader* s, glm::vec3 pos_glo,
	glm::vec3 tlf, glm::vec3 trf, glm::vec3 blf, glm::vec3 brf,
	glm::vec3 tlb, glm::vec3 trb, glm::vec3 blb, glm::vec3 brb,
	GLfloat width, GLfloat height, GLfloat depth)
{
	shader = s;
	position_global = pos_glo;
	glGenVertexArrays(1, &VAO);

	//function for calculating one plane's vertices (and also vertice data)
	auto plane = [](
		glm::vec3 p1, glm::vec3 p2,
		glm::vec3 p3, glm::vec3 p4,

		glm::vec3 normal,
		
		glm::vec2 uv1, glm::vec2 uv2,
		glm::vec2 uv3, glm::vec2 uv4)
	{
		glm::vec3 n = normal;

		//calculate Bi/Tangent for both triangles
		glm::vec3 t1, bt1, t2, bt2;
		//triangle 1
		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		t1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		t1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		t1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		t1 = glm::normalize(t1);

		bt1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bt1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bt1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bt1 = glm::normalize(bt1);

		//triangle 2
		edge1 = p3 - p1;
		edge2 = p4 - p1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		t2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		t2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		t2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		t2 = glm::normalize(t2);

		bt2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bt2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bt2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bt2 = glm::normalize(bt2);

		std::vector<GLfloat> v({
			//position		  //texCoords    //normals		 //Tangent		    //Bitangent
			p1.x, p1.y, p1.z, uv1.x, uv1.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			p2.x, p2.y, p2.z, uv2.x, uv2.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			p3.x, p3.y, p3.z, uv3.x, uv3.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,

			p1.x, p1.y, p1.z, uv1.x, uv1.y,  n.x, n.y, n.z,  t2.x, t2.y, t2.z,  bt2.x, bt2.y, bt2.z,
			p3.x, p3.y, p3.z, uv3.x, uv3.y,  n.x, n.y, n.z,  t2.x, t2.y, t2.z,  bt2.x, bt2.y, bt2.z,
			p4.x, p4.y, p4.z, uv4.x, uv4.y,  n.x, n.y, n.z,  t2.x, t2.y, t2.z,  bt2.x, bt2.y, bt2.z,
		});
		return v;
	};

	std::vector<GLfloat> front = plane(tlf, blf, brf, trf, glm::vec3(0, 0, 1),	glm::vec2(0, height), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, height));
	std::vector<GLfloat> back =  plane(trb, brb, blb, tlb, glm::vec3(0, 0, -1),	glm::vec2(0, height), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, height));

	std::vector<GLfloat> left =  plane(tlb, blb, blf, tlf, glm::vec3(-1, 0, 0),	glm::vec2(0, height), glm::vec2(0, 0), glm::vec2(depth, 0), glm::vec2(depth, height));
	std::vector<GLfloat> right = plane(trf, brf, brb, trb, glm::vec3(1, 0, 0), 	glm::vec2(0, height), glm::vec2(0, 0), glm::vec2(depth, 0), glm::vec2(depth, height));

	std::vector<GLfloat> top =	 plane(tlb, tlf, trf, trb, glm::vec3(0, 1, 0), 	glm::vec2(0, depth), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, depth));
	std::vector<GLfloat> bottom= plane(blf, blb, brb, brf, glm::vec3(0, -1, 0),	glm::vec2(0, depth), glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(width, depth));


	vertices = front;
	vertices.insert(vertices.end(), left.begin(), left.end());

	vertices.insert(vertices.end(), back.begin(), back.end());
	vertices.insert(vertices.end(), top.begin(), top.end());

	vertices.insert(vertices.end(), right.begin(), right.end());
	vertices.insert(vertices.end(), bottom.begin(), bottom.end());

	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//copy data to the buffer
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
			//set vertex attribute pointers
			//position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//texture coordinates
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*3));
			glEnableVertexAttribArray(1);
			//normals
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*5));
			glEnableVertexAttribArray(2);
			//tangent
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*8));
			glEnableVertexAttribArray(3);
			//bitangent
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (GLvoid*)(sizeof(GLfloat)*11));
			glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Cube::Cube(Shader* s, glm::vec3 pos_glo, GLfloat width, GLfloat height, GLfloat depth, glm::vec3 pos_loc) : Cube(
	s, pos_glo, glm::vec3(-width/2, height/2, depth/2), glm::vec3(width/2, height/2, depth/2), glm::vec3(-width/2, -height/2, depth/2), glm::vec3(width/2, -height/2, depth/2), 
		glm::vec3(-width/2, height/2, -depth/2), glm::vec3(width/2, height/2, -depth/2), glm::vec3(-width/2, -height/2, -depth/2), glm::vec3(width/2, -height/2, -depth/2),
			width, height, depth) {}

void Cube::render()
{
	glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}

Cube::~Cube()
{
}
