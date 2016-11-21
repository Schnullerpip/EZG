#include "Cube.h"
#include <vector>

Cube::Cube(Shader* s, glm::vec3 pos_glo,
	glm::vec3 tlf, glm::vec3 trf, glm::vec3 blf, glm::vec3 brf,
	glm::vec3 tlb, glm::vec3 trb, glm::vec3 blb, glm::vec3 brb)
{
	shader = s;
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

		t2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		t2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		t2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		t2 = glm::normalize(t1);

		bt2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bt2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bt2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bt2 = glm::normalize(bt1);

		std::vector<GLfloat> v({
			//position		  //texCoords    //normals		 //Tangent		    //Bitangent
			p1.x, p1.y, p1.z, uv1.x, uv1.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			p2.x, p2.y, p2.x, uv2.x, uv2.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			p3.x, p3.y, p3.x, uv3.x, uv3.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,

			p1.x, p1.y, p1.z, uv1.x, uv1.y,  n.x, n.y, n.z,  t2.x, t2.y, t2.z,  bt2.x, bt2.y, bt2.z,
			p3.x, p3.y, p3.x, uv3.x, uv3.y,  n.x, n.y, n.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			p4.x, p4.y, p4.x, uv4.x, uv4.y,  n.x, n.y, n.z,  t2.x, t2.y, t2.z,  bt2.x, bt2.y, bt2.z,
		});
		return v;
	};

	std::vector<GLfloat> front = plane(tlf, blf, brf, trf, glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));
	std::vector<GLfloat> back =  plane(tlb, blb, brb, trb, glm::vec3(0, 0, -1),glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));

	std::vector<GLfloat> left =  plane(tlb, blb, blf, tlf, glm::vec3(-1, 0, 0),glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));
	std::vector<GLfloat> right = plane(trf, brf, brb, trb, glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));

	std::vector<GLfloat> top =	 plane(tlb, tlf, trf, trb, glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));
	std::vector<GLfloat> bottom= plane(blf, blf, brb, brf, glm::vec3(0, -1, 0),glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1));


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
