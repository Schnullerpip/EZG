#include "Light.h"
#include "Cube.h"
#include <glm/gtc/matrix_transform.inl>

//BAD!! NEVER statically INITIALIZE GLEW STUFF!!! GLEW INIT WILL NOT BE CALLED AT THIS TIME!!!
//Shader* Light::defaultShader = new Shader("src/Shaders/Light_And_Shadow/VertexLight.vs", "src/Shaders/Light_And_Shadow/FragmentLight.fs");

void Light::apply_to(Shader* s) const {
	glUniform3f(glGetUniformLocation(s->Program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glm::vec3 o_pos = object->getPosition();
	glUniform3f(glGetUniformLocation(s->Program, "lightPosition"), o_pos.x, o_pos.y, o_pos.z);
	glUniform1f(glGetUniformLocation(s->Program, "farPlane"), far);
}

Shader* Light::renderShadow(std::vector<Shape*> shapes, GLFWwindow* window) {
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat near = 0.1f;

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.f), aspect, near, far);
	std::vector<glm::mat4> shadowTransforms;

	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));

	// 1. Render scene to depth cubemap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		depthShader->Use();
		for (GLuint i = 0; i < 6; ++i)
			glUniformMatrix4fv(glGetUniformLocation(depthShader->Program, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
		glUniform1f(glGetUniformLocation(depthShader->Program, "farPlane"), far);
		glUniform3fv(glGetUniformLocation(depthShader->Program, "lightPos"), 1, &getPosition()[0]);

		for (auto& s : shapes) {
			glm::mat4 model;
			model = glm::translate(model, s->getPosition());
			model = glm::rotate(model, s->rotation_angle, s->rotation_axis);
			//TODO scale 
			glUniformMatrix4fv(glGetUniformLocation(depthShader->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(s->VAO);
			glDrawArrays(GL_TRIANGLES, 0, s->getVertices()->size());
			glBindVertexArray(0);
		}
	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 //glfwgetwindowsize access violation -> why?!
	 glViewport(0, 0, 800, 600);

	 return depthShader;
}

GLuint Light::renderShadowVSM(std::vector<Shape*> shapes, GLFWwindow* window)
{
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat near = 0.1f;

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.f), aspect, near, far);
	std::vector<glm::mat4> shadowTransforms;

	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(getPosition(), getPosition() + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));

	// 1. Render scene to depth cubemap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOVSM);
		glClearColor(1.f, 0.f, 0.f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		depthShaderVSM->Use();
		for (GLuint i = 0; i < 6; ++i)
			glUniformMatrix4fv(glGetUniformLocation(depthShaderVSM->Program, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
		glUniform1f(glGetUniformLocation(depthShaderVSM->Program, "farPlane"), far);
		glUniform3fv(glGetUniformLocation(depthShaderVSM->Program, "lightPos"), 1, &getPosition()[0]);

		for (auto& s : shapes) {
			glm::mat4 model;
			model = glm::translate(model, s->getPosition());
			model = glm::rotate(model, s->rotation_angle, s->rotation_axis);
			//TODO scale 
			glUniformMatrix4fv(glGetUniformLocation(depthShaderVSM->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(s->VAO);
			glDrawArrays(GL_TRIANGLES, 0, s->getVertices()->size());
			glBindVertexArray(0);
		}

	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 glViewport(0, 0, 800, 600);

	 return depthCubeMapVSM;
}
void Light::update(GLfloat deltaTime)
{
}

void Light::render()
{
	// Don't forget to 'use' the corresponding shader program first (to set the uniform)
	shader->Use();
	glUniform3f(glGetUniformLocation(shader->Program, "lightColor"),  lightColor.x, lightColor.y, lightColor.z); // Also set light's color (white)

	object->render();
}



Light::Light(Shader* s, glm::vec3 position, GLfloat width, GLfloat height, GLfloat depth) {
	lightColor = glm::vec3(1.f, 1.f, 1.f);
	shader = s;
	depthShader = 
		new Shader( "src/Shaders/Light_And_Shadow/pointShadow.vs",
					"src/Shaders/Light_And_Shadow/pointShadow.fs",
					"src/Shaders/Light_And_Shadow/pointShadow.gs");

	depthShaderVSM = 
		new Shader( "src/Shaders/Light_And_Shadow/pointShadow.vs",
					"src/Shaders/Light_And_Shadow/pointShadowVSM.fs",
					"src/Shaders/Light_And_Shadow/pointShadow.gs");

	object = new Cube(shader, position, width, height, depth);

	//stuff for shadow
	glGenTextures(1, &depthCubeMap);
	glGenTextures(1, &depthCubeMapVSM);
	glGenFramebuffers(1, &depthMapFBO);
	glGenFramebuffers(1, &depthMapFBOVSM);

	//-------------------------------------------------------VSM shadow
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapVSM);
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RG32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOVSM);
		//attach depthCubeMap (texture) to the framebuffer so it gets written
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, depthCubeMapVSM, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		//glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);







	//-------------------------------------------------------normal shadow
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		//attach depthCubeMap (texture) to the framebuffer so it gets written
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
		//tell opengl to not render to a color buffer! (we only need the depth values)
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Light::~Light() {
	//delete object;
}