#include "Light_And_Shadow.h"
#include "Cube.h"
#include <glm/gtx/quaternion.hpp>
#include "Text.h"


bool light_follow = false;
bool increase_normal_effect = false;
bool decrease_normal_effect = false;
GLfloat bump_factor = 1.f;

void Light_And_Shadow::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

}

Light_And_Shadow::Light_And_Shadow(Input_Handler* i, int number_samps)
{
	input = i;
	number_samples = number_samps;

	//shaders
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexShader.vs","src/Shaders/Light_And_Shadow/FragmentShader.fs"));
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexLight.vs", "src/Shaders/Light_And_Shadow/FragmentLight.fs"));
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexShader.vs","src/Shaders/Light_And_Shadow/FragmentShader_Light_Affected.fs"));
	//for normal mapping
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/smartVertex.vs","src/Shaders/Light_And_Shadow/smartFragment.fs"));

	shader[2]->Use();
	glUniform1i(glGetUniformLocation(shader[2]->Program, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(shader[2]->Program, "depthMap"), 1);

	shader[3]->Use();
	glUniform1i(glGetUniformLocation(shader[3]->Program, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(shader[3]->Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(shader[3]->Program, "depthMap"), 2);


	texture.push_back(new Texture("images/crackedsoil.jpg"));
	texture.push_back(new Texture("images/crackedsoil_NRM.png"));

	texture.push_back(new Texture("images/crate.jpg"));
	texture.push_back(new Texture("images/crate_NRM.png"));

	texture.push_back(new Texture("images/grass.jpg"));
	texture.push_back(new Texture("images/grass_NRM.png"));

	//objects
	shape.push_back(new Cube(shader[3], glm::vec3(0,-0.2,0), 100.f, 0.4f, 100.f));
	shape.push_back(new Cube(shader[3], glm::vec3(0,20,0), 5, 5, 5));
	shape.push_back(new Cube(shader[3], glm::vec3(10,10,0), 10, 5, 10));
	shape.push_back(new Cube(shader[3], glm::vec3(-50,0,0), 0.4f, 50, 100));
	shape.push_back(new Cube(shader[3], glm::vec3(0,10,10), glm::vec3(-0.5f, 0.5f, 0.5f)));
	shape.push_back(new Cube(shader[3], glm::vec3(-5,10,5), glm::vec3(-0.5f, 0.5f, 0.5f)));
	shape.push_back(new Cube(shader[3], glm::vec3(-5, 20, 30), glm::vec3(-0.5f, 0.5f, 0.5f)));

	//lights
	light.push_back(new Light(shader[1], glm::vec3(6, 5.5, 5)));

	//start position for camera
	cam.pos = glm::vec3(5, 20, 50);

	shape[0]->texture = texture[4];
	shape[1]->texture = texture[2];
	shape[2]->texture = texture[2];
	shape[3]->texture = texture[0];
	shape[4]->texture = texture[0];
	shape[5]->texture = texture[2];
	shape[6]->texture = texture[0];

	shape[0]->normalMap = texture[5];
	shape[1]->normalMap = texture[3];
	shape[2]->normalMap = texture[3];
	shape[3]->normalMap = texture[1];
	shape[4]->normalMap = texture[1];
	shape[5]->normalMap = texture[3];
	shape[6]->normalMap = texture[1];

	console = OnScreenConsole(3.2f, input, 800, 600);
	input->subscribe(&console);
	std::stringstream ss;
	ss << "MSAA -> using " << number_samps << " samples";
	console.out(new OnScreenMessage(ss.str(), glm::vec3(0.5f, 0.8f, 0.3f), 0.3f));
}

void Light_And_Shadow::render(GLfloat deltaTime) {

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

	int i;
	for ( i = 0; i < shape.size(); ++i)
	{
		cam.model_translation(shape[i]->getPosition());
		cam.apply_to(shape[i]->shader);
		if(increase_normal_effect || decrease_normal_effect)
			glUniform1f(glGetUniformLocation(shape[i]->shader->Program, "bumpFactor"), bump_factor);

		light[0]->apply_to(shape[i]->shader);
		glActiveTexture(GL_TEXTURE0);
		shape[i]->texture->use();

		glActiveTexture(GL_TEXTURE1);
		shape[i]->normalMap->use();

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, light[0]->getDepthCubeMap());
		shape[i]->draw();
	}
	console.update(deltaTime);
}

void Light_And_Shadow::update(GLfloat deltaTime, EventFeedback* feedback) {
	std::string message;
	std::stringstream ss;

	light_follow = input->is_pressed(GLFW_KEY_SPACE, false);
	increase_normal_effect = input->is_pressed(GLFW_KEY_K, false);
	decrease_normal_effect = input->is_pressed(GLFW_KEY_J, false) && !increase_normal_effect;

	auto configureSamples = [&](int n) -> std::string {
		feedback->setNumberSamples(n);
		ss << "[MSAA]::Samples -> configured to " << n << " - press return to apply changes";
		return ss.str();
	};

	//check if the antialiasing properties should be modified (in feedback)
	if (input->is_pressed(GLFW_KEY_0))
		message = configureSamples(0);
	else if (input->is_pressed(GLFW_KEY_1))
		message = configureSamples(1);
	else if (input->is_pressed(GLFW_KEY_2))
		message = configureSamples(2);
	else if (input->is_pressed(GLFW_KEY_3))
		message = configureSamples(3);
	else if (input->is_pressed(GLFW_KEY_4))
		message = configureSamples(4);
	else if (input->is_pressed(GLFW_KEY_5))
		message = configureSamples(5);
	else if (input->is_pressed(GLFW_KEY_6))
		message = configureSamples(6);
	else if (input->is_pressed(GLFW_KEY_7))
		message = configureSamples(7);
	else if (input->is_pressed(GLFW_KEY_8))
		message = configureSamples(8);

	if (!message.empty())
	{
		console.out(new OnScreenMessage(message));
	}


	//check if the game should restart -> for example after configuring antialiasing properties (new context needed)
	if (feedback->restart = input->is_pressed(GLFW_KEY_ENTER))
	{
		feedback->quitgame = false;
	}

	//check if normalmap specific user input appeared
	if (increase_normal_effect)
	{
		bump_factor += .01f;
		ss << "[NormalMaps]::Bumpyness factor set to: " << bump_factor;
		console.out(new OnScreenMessage(ss.str()));

	}
	else if (decrease_normal_effect)
	{
		bump_factor -= .01f;
		ss << "[NormalMaps]::Bumpyness factor set to: " << bump_factor;
		console.out(new OnScreenMessage(ss.str()));
	}

	//apply mouse movement to the camera
	cam.update_fps_style(deltaTime, input);

}

Light_And_Shadow::~Light_And_Shadow()
{
}
