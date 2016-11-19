#include "Kochanek_Bartels_Scene.h"
#include "Cube.h"
#include "Pyramid.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


using glm::vec3;
using glm::quat;
using glm::angleAxis;

void Kochanek_Bartels_Scene::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

Kochanek_Bartels_Scene::Kochanek_Bartels_Scene() {

	Kochanek_Bartels_Scene::init(800, 600, "Übung 1 - Kochanek Bartels/Slerp/Squad");
	shader.push_back(new Shader("src/Shaders/vertexShader.vs", "src/Shaders/simpleFragmentShader.fs"));
	shader.push_back(new Shader("src/Shaders/vertexShader.vs", "src/Shaders/KB_position.fs"));

	currentPosition = new Cube(shader[1]);

	shape.push_back(new Cube(shader[0]));
	shape.push_back(new Cube(shader[0], glm::vec3(1, 2, 3), 3, 3, 3));
	shape.push_back(new Pyramid(glm::vec3(-5, -10, 30), 2, -20, 2));
	shape.push_back(new Cube(shader[0], glm::vec3(7, 5, -30), 3, 3, 3));
	shape.push_back(new Cube(shader[0], glm::vec3(-7, -5, 30), 3, 3, 3));
	shape.push_back(new Pyramid(glm::vec3(0, -5, -10), 2, 3, 2));
	shape.push_back(new Cube(shader[0], glm::vec3(7, 5, -3), 3, 3, 10));
	shape.push_back(new Cube(shader[0], glm::vec3(0, -15, -55), 3, -20, 10));
	shape.push_back(new Cube(shader[0], glm::vec3(0, -5, -30), 3, 3, 3));
	shape.push_back(new Pyramid(glm::vec3(0, -3.5, -30), 3, 2, 3));
	shape.push_back(new Cube(shader[0], glm::vec3(0, -5, -45), 8, 8, 8));
	shape.push_back(new Pyramid(glm::vec3(10, -10, -50), 10, 10, 10));

	checkpoints = {
		glm::vec3(-10.f, -20.f, 80.f), 
		glm::vec3(-10.f, 0.f, 50.f), 
		glm::vec3(0.f, 5.f, 20.f), 
		glm::vec3(-3.f, 5.f, 0.f), 
		glm::vec3(0.f, 8.f, -20.f), 
		glm::vec3(13.f, 10.f, -40.f),
		glm::vec3(5.f, 5.f, -60.f), 
		glm::vec3(0.f, -25.f, -80.f), 
		glm::vec3(2.f, -30.f, -70.f), 
		glm::vec3(10.f, -33.f, -80.f), 
		glm::vec3(8.f, -12.f, -90.f)
	};
	slerp.add_quat(0.f, vec3(0.f, 0.f, 1.f));
	slerp.add_quat(30.f, vec3(1.f, 0.f, 1.f));
	slerp.add_quat(20.f, vec3(0.f, 1.f, 0.f));
	slerp.add_quat(50.f, vec3(0.f, 1.f, 0.f));
	slerp.add_quat(10.f, vec3(0.f, 1.f, 1.f));
	slerp.add_quat(25.f, vec3(1.f, 0.f, 0.f));
	slerp.add_quat(40.f, vec3(1.f, 0.f, 0.f));
	slerp.add_quat(10.f, vec3(0.f, 1.f, 1.f));
	slerp.add_quat(30.f, vec3(0.f, 0.f, 1.f));
	slerp.add_quat(50.f, vec3(0.f, 0.f, 1.f));
	slerp.add_quat(70.f, vec3(0.f, 0.f, 1.f));

	//slerp.add_quat(0.f, vec3(0.1f, 1.f, 1.f));
	//slerp.add_quat(-7.f, vec3(0.f, 1.f, 1.f));
	//slerp.add_quat(10.f, vec3(0.f, 1.f, 1.f));
	//slerp.add_quat(15.f, vec3(1.f, 1.f, 1.f));
	//slerp.add_quat(10.f, vec3(1.f, 1.f, 1.f));
	//slerp.add_quat(20.f, vec3(1.f, 0.f, 0.f));
	//slerp.add_quat(4.f, vec3(1.f, 0.f, 0.f));
	//slerp.add_quat(0.f, vec3(0.f, 1.f, 0.f));
	//slerp.add_quat(0.f, vec3(0.f, 1.f, 0.f));
	//slerp.add_quat(0.f, vec3(0.f, 1.f, 0.f));
	//slerp.add_quat(0.f, vec3(0.f, 1.f, 0.f));

	kbi = new KB_Interpolation(checkpoints);
}
void Kochanek_Bartels_Scene::render()
{
	glm::vec3 cP = kbi->step();
	glm::mat4 view_factor;
	view_factor = glm::toMat4(slerp.step(kbi->getP(), kbi->getT()));
	cam.view(glm::vec3(cP.x, cP.y, cP.z + 10), cam.front , cam.up, view_factor);
	cam.projection_p(800, 600, 0.1f, 300.f);

	cam.model(cP, 45, glm::vec3(1.f, 0.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f));
	cam.apply_to(shader[1]);
	currentPosition->draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//render some shapes to butify the scene
	for (unsigned i = 1; i < shape.size(); i++) {
		cam.model(shape[i]->getPosition());
		cam.apply_to(shader[1]);
		shape[i]->draw();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//render all the Checkpoints as cubes
	for (auto c : checkpoints) {
		cam.model(c);
		cam.apply_to(shader[0]);
		shape[0]->draw();
	}
}

void Kochanek_Bartels_Scene::update(GLfloat deltaTime)
{
}

Kochanek_Bartels_Scene::~Kochanek_Bartels_Scene()
{
	delete kbi;
}

