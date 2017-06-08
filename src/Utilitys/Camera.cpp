#include "Camera.h"
#include <GLFW/glfw3.h>
#include "Input_Handler.h"

void Camera::apply_to(Shader* shader)
{
	shader->Use();
	GLint modelLoc = glGetUniformLocation(shader->Program, "model");
	GLint viewLoc = glGetUniformLocation(shader->Program, "view");
	GLint projLoc = glGetUniformLocation(shader->Program, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_tmp));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_tmp));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_tmp));

	glUniform3f(glGetUniformLocation(shader->Program, "cameraPosition"), pos.x, pos.y, pos.z);
	reset_model_tmp();
}

void Camera::update_fps_style(GLfloat deltaTime, Input_Handler* input, GLfloat camSpeed) {
	GLfloat cameraSpeed = this->camSpeed * deltaTime;

	//keys
	if (input->is_pressed(GLFW_KEY_W, false)) {
		pos += cameraSpeed * front;
	}
    if (input->is_pressed(GLFW_KEY_S, false))
        pos -= cameraSpeed * front;
    if (input->is_pressed(GLFW_KEY_A, false))
        pos -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (input->is_pressed(GLFW_KEY_D, false))
        pos += glm::normalize(glm::cross(front, up)) * cameraSpeed;	

	//mouse
	GLfloat sensitivity = 0.5;	// Change this value to your liking
    input->xoffset *= sensitivity;
    input->yoffset *= sensitivity;

    yaw   += input->xoffset;
    pitch += input->yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
}

glm::mat4 Camera::view(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::mat4 factor)
{
	glm::mat4 v;
	view_tmp = v;
	view_tmp =factor * glm::lookAt(position, position + front, up);
	return view_tmp;
}

Camera::Camera()
{
}


Camera::~Camera()
{
}
