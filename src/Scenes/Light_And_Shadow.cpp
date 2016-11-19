#include "Light_And_Shadow.h"
#include "Cube.h"
#include <glm/gtx/quaternion.hpp>

bool light_follow = false;

void Light_And_Shadow::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

Light_And_Shadow::Light_And_Shadow(Input_Handler* i)
{
	input = i;

	//shaders
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexShader.vs","src/Shaders/Light_And_Shadow/FragmentShader.fs"));
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexLight.vs", "src/Shaders/Light_And_Shadow/FragmentLight.fs"));
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexShader.vs","src/Shaders/Light_And_Shadow/FragmentShader_Light_Affected.fs"));

	shader[2]->Use();
	glUniform1i(glGetUniformLocation(shader[2]->Program, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(shader[2]->Program, "depthMap"), 1);


	texture.push_back(new Texture("images/crackedsoil.jpg"));
	texture.push_back(new Texture("images/wood.jpg"));
	texture.push_back(new Texture("images/grass.jpg"));

	//objects
	shape.push_back(new Cube(shader[2], glm::vec3(0,20,0), 5, 5, 5));
	shape.push_back(new Cube(shader[2], glm::vec3(0,-0.2,0), 100.f, 0.4f, 100.f));
	shape.push_back(new Cube(shader[2], glm::vec3(10,10,0), 10, 5, 10));
	shape.push_back(new Cube(shader[2], glm::vec3(-50,0,0), 0.4f, 50, 100));
	shape.push_back(new Cube(shader[2], glm::vec3(13,10,0), 2, 10, 2));
	shape.push_back(new Cube(shader[2], glm::vec3(-5,10,5), 5, 2, 2));

	//lights
	light.push_back(new Light(shader[1], glm::vec3(6, 5.5, 5)));

	//start position for camera
	cam.pos = glm::vec3(5, 20, 50);


	shape[0]->texture = texture[0];
	shape[1]->texture = texture[2];
	shape[2]->texture = texture[1];
	shape[3]->texture = texture[0];
	shape[4]->texture = texture[0];
	shape[5]->texture = texture[1];
}

void Light_And_Shadow::render() {

	//first render shadow
	light[0]->renderShadow(shape, window);

	cam.view();
	cam.projection_p(800,600);

	for (auto& l : light) { //only one though
		if (light_follow) {
			//set light ten units infront of the camera
			glm::vec3 posi = cam.pos + (glm::vec3(10, 10, 10) * cam.front);
			l->setPosition(posi);
			cam.model_translation(posi);
		}
		else {
			cam.model_translation(l->getPosition());
		}
		cam.apply_to(l->getShader());
		l->render();
	}

	for (auto& s : shape) {
		cam.model_translation(s->getPosition());
		cam.apply_to(s->shader);
		light[0]->apply_to(s->shader);
		glActiveTexture(GL_TEXTURE0);
		s->texture->use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, light[0]->getDepthCubeMap());
		s->draw();
	}
}

void Light_And_Shadow::update(GLfloat deltaTime) {
	light_follow = input->is_pressed(GLFW_KEY_SPACE);
	cam.update_fps_style(deltaTime, input);
}

Light_And_Shadow::~Light_And_Shadow()
{
}
