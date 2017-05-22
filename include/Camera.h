#pragma once
// GLM Mathematics
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Input_Handler.h"

class Camera
{

public:
	glm::mat4 view_tmp;
	glm::mat4 projection_tmp;
	glm::mat4 model_tmp;
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	GLfloat pitch = 0.0f;
	GLfloat fov = 45.0f;

	glm::mat4 view(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::mat4 factor = glm::mat4());

	void reset_model_tmp()
	{
		model_tmp = glm::mat4();
	}

	inline glm::mat4 view(glm::mat4 factor = glm::mat4()) {
		glm::mat4 v;
		view_tmp = v;
		view_tmp = glm::lookAt(pos, pos + front, up)*factor;
		return view_tmp;
	}
	inline glm::mat4 projection_p(GLfloat width, GLfloat height, GLfloat near = 0.1f, GLfloat far=1000.f) {
		glm::mat4 p;
		projection_tmp = p;
		projection_tmp = glm::perspective(glm::radians(fov), width/height, near, far);
		return projection_tmp;
	}
	inline glm::mat4 model(glm::vec3 translation = glm::vec3(0, 0, 0),
		GLfloat rotation_angle = 0, glm::vec3 rotation_axis = glm::vec3(1.f, 1.f, 1.f),
		glm::vec3 scalation = glm::vec3(1.f, 1.f, 1.f)) {
		glm::mat4 m;
		model_tmp = m;
		model_tmp = glm::translate(model_tmp, translation);
		model_tmp = glm::rotate(model_tmp, rotation_angle, rotation_axis);
		model_tmp = glm::scale(model_tmp, scalation);
		return model_tmp;
	}

	inline void model_translation(glm::vec3 translation, bool fresh = true) {
		if (fresh) {
			model_tmp = glm::mat4();
		}
		model_tmp = glm::translate(model_tmp, translation);
	}

	inline void model_rotation(GLfloat angle, glm::vec3 axis) {
		glm::mat4 m;
		model_tmp = m;
		model_tmp = glm::rotate(model_tmp, angle, axis);
	}

	inline void model_scalation(glm::vec3 scalation) {
		glm::mat4 m;
		model_tmp = m;
		model_tmp = glm::scale(model_tmp, scalation);
	}

	void apply_to(Shader* shader);

	/*for fps
	apply mouse movement to the camera
	@param deltaTime -> ...yeah its deltaTime
	@param input -> The InputHandler ptr that holds all the information about the mousemovement, we need
	*/
	void update_fps_style(GLfloat deltaTime, Input_Handler* input, GLfloat camSpeed = 40.f);

	Camera();
	~Camera();
};

