#include "KB_Interpolation.h"
#include <iostream>
#include <GLFW/glfw3.h>


vec3 KB_Interpolation::step()
{
	//calculate delta time
	double new_frame = glfwGetTime();
	double delta_time = new_frame - last_frame;
	last_frame = new_frame;

	//hermite
	vec3 returnVal = hermite(t, current_p);
	//GLfloat distance_factor = distances[current_p] / total_distance;
	GLfloat step_size = interval * delta_time * speed;
	t += step_size;
	total_t += step_size;
	if (t >= 1) {
		t = 0;
		if(++current_p >= checkpoints.size()-1)
			current_p = 0;
	}
	return returnVal;
}

KB_Interpolation::KB_Interpolation(std::vector<vec3>& c) {
	checkpoints = c;

	for (int i = 1; i < checkpoints.size(); i++) {
		GLfloat distance = (checkpoints[i] - checkpoints[i - 1]).length();
		distances.push_back(distance);
		total_distance += distance;
	}
}

glm::vec3 KB_Interpolation::hermite(GLfloat t, int p_i)
{
	vec3 pi = checkpoints[p_i];
	vec3 pi1 = checkpoints[p_i + 1];

	vec3 pi_min1;
	vec3 pi_plus_1;
	
	pi_min1 = (p_i == 0) ? checkpoints[0] : checkpoints[p_i - 1];
	pi_plus_1 = (p_i + 1 == checkpoints.size()-1) ? checkpoints[checkpoints.size()-1] : checkpoints[p_i + 2];

	GLfloat ten_src, bia_src, cont_src;
	GLfloat ten_des, bia_des, cont_des;
	ten_src = bia_src = cont_src = 0;
	ten_des = bia_des = cont_des = 0;

	// if the step is a big part of the distance the effects should be lowered
	GLfloat distance_factor = distances[p_i] / total_distance;

	GLfloat interval_factor_src = interval*speed / distances[p_i];
	GLfloat interval_factor_dest = interval*speed / distances[(p_i == distances.size()-1) ? p_i : p_i+1];

	//ten_des = interval_factor_dest;
	//ten_src = interval_factor_src * -1;

	cont_src = 1 - distance_factor;
	cont_des = cont_src *-1;

	return (2 * t*t*t - 3 * t*t + 1)*pi +
		(t*t*t - 2 * t*t + t)*tangent_dest(pi_min1, pi, pi1, ten_des, bia_des, cont_des) +
		(t*t*t - t*t)*tangent_src(pi, pi1, pi_plus_1, ten_src, bia_src, cont_src) +
		(-2 * t*t*t + 3 * t*t)*pi1;
}

glm::vec3 KB_Interpolation::tangent_src(vec3 p0, vec3 p1, vec3 p2, GLfloat tension, GLfloat bias, GLfloat continuity)
{
	return ((1.f - tension)*(1.f + bias)*(1.f - continuity)) / 2 * (p1 - p0) +
		((1.f - tension)*(1.f - bias)*(1.f + continuity)) / 2 * (p2 - p1);
}

glm::vec3 KB_Interpolation::tangent_dest(vec3 p0, vec3 p1, vec3 p2, GLfloat tension, GLfloat bias, GLfloat continuity)
{
	return ((1.f - tension)*(1.f + bias)*(1.f + continuity)) / 2 * (p1 - p0) +
		((1.f - tension)*(1.f - bias)*(1.f - continuity)) / 2 * (p2 - p1);
}

KB_Interpolation::~KB_Interpolation()
{
}