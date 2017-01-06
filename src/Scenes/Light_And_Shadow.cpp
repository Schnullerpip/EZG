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

//helper function for recursively initializing boundingboxrepresentations according to the kd_tree
void initialize_bbr(std::vector<std::pair<Cube*, glm::vec3>>* cubes, std::vector<BoundingBox*>* bboxes, Node* node, Shader* s, int depth = 0)
{
	if (!node)return;
	BoundingBox* bb = node->bbox;
	if (!bb)return;
	float c = 0.13 * depth;
	cubes->push_back(std::make_pair(new Cube(s, bb->getPosition(), bb->Width(), bb->Height(), bb->Depth()), glm::vec3(c, c, c))) ;
	(*bboxes).push_back(bb);
	initialize_bbr(cubes, bboxes, node->left, s, depth +1);
	initialize_bbr(cubes, bboxes, node->right, s, depth +1);
}

Light_And_Shadow::Light_And_Shadow(Input_Handler* i, EventFeedback* fb)
{
	input = i;
	feedback = fb;

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

	Cube* floor =		new Cube(shader[3], glm::vec3(0, 0, -15), 5.f, 5.f, 1.f);
	Cube* highest_cube =new Cube(shader[3], glm::vec3(0,0,-10), 1, 1, 1);
	Cube* cube2 =		new Cube(shader[3], glm::vec3(10,0,-10), 1, 2, 1);
	Cube* wall =		new Cube(shader[3], glm::vec3(-10,0,0), 0.4f, 8, 1);
	Cube* unit_cube =	new Cube(shader[3], glm::vec3(10,-2,4), 1, 1, 1);
	Cube* cube5 =		new Cube(shader[3], glm::vec3(-5,10,5), 0.5, 0.5, 0.5);

	Cube* cube6 = new Cube(shader[3], glm::vec3(-5, 20, 30), glm::vec3(-0.5f, 0.5f, 0.5f));

	floor->name = "floor";
	highest_cube->name = "highest_cube";
	cube2->name = "cube2";
	wall->name = "wall";

	unit_cube->name = "unit_cube";

	//objects
	shape.push_back(floor);
	shape.push_back(highest_cube);
	shape.push_back(cube2);
	shape.push_back(wall);
	//shape.push_back(unit_cube);
	//shape.push_back(cube5);
	//shape.push_back(cube6);

	//lights
	light.push_back(new Light(shader[1], glm::vec3(6, 5.5, 5)));

	//start position for camera
	cam.pos = glm::vec3(5, 20, 50);

	floor->texture = texture[4];
	highest_cube->texture = texture[2];
	cube2->texture = texture[2];
	wall->texture = texture[0];
	unit_cube->texture = texture[0];
	cube5->texture = texture[2];
	cube6->texture = texture[0];

	floor->normalMap = texture[5];
	highest_cube->normalMap = texture[3];
	cube2->normalMap = texture[3];
	wall->normalMap = texture[1];
	unit_cube->normalMap = texture[1];
	cube5->normalMap = texture[3];
	cube6->normalMap = texture[1];


	//initialize the OnScreenConsole
	console = new OnScreenConsole(1.5f, input, feedback, 800, 600);
	input->subscribe(console); //events come from observerpattern
	std::stringstream ss;
	ss << "MSAA -> using " << feedback->number_samples << " samples";
	console->out(new OnScreenMessage(ss.str()));

	//initialize the KD_Tree
	kdt = new KD_Tree(3, shape);
	std::vector<BoundingBox*> bboxes;
	initialize_bbr(&boundingBoxRepresentation, &bboxes, kdt->Root(), shader[1]);
	ss.clear();
	ss << "[Scene]::Initialized KD_Tree -> size: " << kdt->Size();
	console->out(ss.str());
}

void renderBB(std::vector<std::pair<Cube*, glm::vec3>>& cubes, Camera* cam)
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	/*render all the boundingbox representations*/
	for (auto bb : cubes)
	{
		cam->model_translation(bb.first->getPosition());
		cam->apply_to(bb.first->shader);
		glUniform3f(glGetUniformLocation(bb.first->shader->Program, "lightColor"),  bb.second.x, bb.second.y, bb.second.z);
		bb.first->draw();
	}
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void Light_And_Shadow::render(GLfloat deltaTime) {

	//first render shadow
	light[0]->renderShadow(shape, window);

	cam.view();
	cam.projection_p(800,600);

	//render the light sources
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

	//render the primitives
	int i;
	for ( i = 0; i < shape.size(); ++i)
	{
		cam.model_translation(shape[i]->getPosition());
		cam.apply_to(shape[i]->shader);
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

	//render the bounding boxes
	renderBB(boundingBoxRepresentation, &cam);

	console->update(deltaTime);
}

void Light_And_Shadow::update(GLfloat deltaTime, EventFeedback* feedback) {
	if (console->isInInsertMode())return;
	std::string message;
	std::stringstream ss;

	light_follow = input->is_pressed(GLFW_KEY_SPACE, false);
	increase_normal_effect = input->is_pressed(GLFW_KEY_K, false);
	decrease_normal_effect = input->is_pressed(GLFW_KEY_J, false) && !increase_normal_effect;

	//check if normalmap specific user input appeared
	if (increase_normal_effect)
	{
		bump_factor += .01f;
		ss << "[Bumpyness]::set to: " << bump_factor;
		console->out(new OnScreenMessage(ss.str()));

	}
	else if (decrease_normal_effect)
	{
		bump_factor -= .01f;
		ss << "[Bumpyness]::set to: " << bump_factor;
		console->out(new OnScreenMessage(ss.str()));
	}

	//apply mouse movement to the camera
	cam.update_fps_style(deltaTime, input);
}

Light_And_Shadow::~Light_And_Shadow()
{
	for (auto t : texture)
		delete t;
	for (auto s : shader)
		delete s;
	for (auto s : shape)
		delete s;
	delete console;
}

void Light_And_Shadow::draw_bbox(BoundingBox* boundingbox, Camera* cam, Shader* s)
{
	if (!boundingbox)
    return;

  // Cube 1x1x1, centered on origin
  GLfloat vertices[] = {
    -0.5, -0.5, -0.5, 1.0,
     0.5, -0.5, -0.5, 1.0,
     0.5,  0.5, -0.5, 1.0,
    -0.5,  0.5, -0.5, 1.0,
    -0.5, -0.5,  0.5, 1.0,
     0.5, -0.5,  0.5, 1.0,
     0.5,  0.5,  0.5, 1.0,
    -0.5,  0.5,  0.5, 1.0,
  };
  GLuint vbo_vertices;
  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GLushort elements[] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    0, 4, 1, 5, 2, 6, 3, 7
  };
  GLuint ibo_elements;
  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


  GLfloat
    min_x, max_x,
    min_y, max_y,
    min_z, max_z;

	min_x = boundingbox->FromX();
	min_y = boundingbox->FromY();
	min_z = boundingbox->FromZ();
	max_x = boundingbox->ToX();
	max_y = boundingbox->ToY();
	max_z = boundingbox->ToZ();

  	glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
  	glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
  	glm::mat4 transform =  glm::translate(glm::mat4(1), center)* glm::scale(glm::mat4(1), size);
	
  	/* Apply object's transformation matrix */
  	//glm::mat4 m = boundingbox->object2world * transform;
	cam->model_translation(center);
	cam->model_scalation(size);
	cam->apply_to(s);
  	//glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));
	
  	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  	glEnableVertexAttribArray(0);
  	glVertexAttribPointer(
  	  0,  // attribute
  	  4,                  // number of elements per vertex, here (x,y,z,w)
  	  GL_FLOAT,           // the type of each element
  	  GL_FALSE,           // take our values as-is
  	  0,                  // no extra data between each position
  	  0                   // offset of first element
  	);
	
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
  	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
  	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
  	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
  	glDisableVertexAttribArray(0);
  	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
  	glDeleteBuffers(1, &vbo_vertices);
  	glDeleteBuffers(1, &ibo_elements);
}
