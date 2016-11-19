#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using glm::vec3;
class KB_Interpolation
{
	GLfloat t = 0;
	GLfloat total_t = 0;
	GLfloat interval = 0.03f;
	GLfloat speed = 10;
	double last_frame = 0;

	int current_p = 0;
	std::vector<GLfloat> distances;
	GLfloat total_distance = 0;
	std::vector<vec3> checkpoints;

	glm::vec3 hermite(GLfloat t, int p_i);
	glm::vec3 tangent_src(vec3 p0, vec3 p1, vec3 p2, GLfloat tension = 0, GLfloat bias = 1.f, GLfloat continuity = 0);
	glm::vec3 tangent_dest(vec3 p0, vec3 p1, vec3 p2, GLfloat tension = 0, GLfloat bias = 0, GLfloat continuity = 0);

public:
	inline void setInterval(GLfloat i) { interval = i; }
	inline GLfloat getInterval() const { return interval; }
	inline GLfloat getT() const { return t; }
	inline int getP() const { return current_p; }
	inline void increaseSpeed() { speed += 1; }
	inline void decreaseSpeed() { speed -= 1; }
	vec3 step();
	KB_Interpolation(std::vector<vec3>& checkpoints);
	~KB_Interpolation();
};

