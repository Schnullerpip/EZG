#include "Light_And_Shadow.h"
#include "Cube.h"
#include <glm/gtx/quaternion.hpp>
#include "Text.h"
#include <stack>


bool light_follow = false;
bool increase_normal_effect = false;
bool decrease_normal_effect = false;
GLfloat bump_factor = 1.f;

void Light_And_Shadow::init(int window_width, int window_height, const char* title) {
	Scene::init(window_width, window_height, title);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

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

	Cube* floor =		new Cube(shader[3], glm::vec3(0, -4, 0), 50.f, 2.f, 50.f);
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
	console->registerCommand(&draw_kd_tree, "DRAW KD", "rendering Kd Tree");
	console->registerCommand(&increase_maxd, "DP", "increased max depth");
	console->registerCommand(&decrease_maxd, "DM", "decreased max depth");
	console->registerCommand(&kd_set_complexity, "COMPLEXITY", "updating KD_TREE's complexity by mouse scroll");
	console->registerCommand(&rebuild_kdt, "REBUILD", "Rebuilding kd tree");

	//initialize the KD_Tree
	kdt = new KD_Tree(3, shape);
	ss.clear();
	ss << "[Scene]::Initialized KD_Tree -> size: " << kdt->Size();
	console->out(ss.str());


}

void renderKD_Node(Node* node, Shader* s, Camera* cam)
{
	if (!node)return;
	Cube* c = new Cube(s, node->bbox->getPosition(), node->bbox->Width(), node->bbox->Height(), node->bbox->Depth());
	cam->model_translation(c->getPosition());
	cam->apply_to(c->shader);
	float color = node->depth * 0.13f;
	glUniform3f(glGetUniformLocation(c->shader->Program, "lightColor"), color, color, color);
	c->draw();

	renderKD_Node(node->left(), s, cam);
	renderKD_Node(node->right(), s, cam);
}
void renderKD(Node* node, Shader* s, Camera* cam, int max_depth, Node* hit_node)
{
	std::stack<Node*> nodes;
	Node* iter = hit_node;
	while (hit_node)
	{
		nodes.push(iter);
		if (iter->parent)
			iter = iter->parent;
		else break;
	}

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	if(hit_node)
		for (int i = 0; i < max_depth && !nodes.empty(); ++i)
		{
			Node* n = nodes.top();
			Cube* c = new Cube(s, n->bbox->getPosition(), n->bbox->Width(), n->bbox->Height(), n->bbox->Depth());
			cam->model_translation(c->getPosition());
			cam->apply_to(c->shader);
			float color = n->depth * 0.13f;
			glUniform3f(glGetUniformLocation(c->shader->Program, "lightColor"), color, color, color);
			c->draw();
			nodes.pop();
		}
	else
		renderKD_Node(node, s, cam);
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
	static int maxd = 0;
	if (increase_maxd) { maxd++; increase_maxd = false;}
	else if (decrease_maxd && (maxd-1) > 0) { maxd--; decrease_maxd = false;}
	if (draw_kd_tree)
	{
		renderKD(kdt->Root(), shader[1], &cam, maxd,hitpoint_node1);
		if (hitpoint_repr1)
		{

			cam.model_translation(hitpoint_repr1->getPosition());
			cam.apply_to(hitpoint_repr1->shader);
			hitpoint_repr1->draw();
		}
		if (hitpoint_repr2)
		{

			cam.model_translation(hitpoint_repr2->getPosition());
			cam.apply_to(hitpoint_repr2->shader);
			hitpoint_repr2->draw();
		}

		//if (hitpoint_repr1 && hitpoint_repr2) {
		//	Shape::drawLine(hitpoint_repr1->getPosition(), hitpoint_repr2->getPosition(), shader[2]);
		//}
	}


	console->update(deltaTime);
}

void Light_And_Shadow::update(GLfloat deltaTime, EventFeedback* feedback) {
	if (console->isInInsertMode())return;
	std::string message;
	std::stringstream ss;

	light_follow = input->is_pressed(GLFW_KEY_SPACE, false);
	increase_normal_effect = input->is_pressed(GLFW_KEY_K, false);
	decrease_normal_effect = input->is_pressed(GLFW_KEY_J, false) && !increase_normal_effect;
	
	if (kd_set_complexity)
	{
		std::stringstream ss;
		kdt->setComplexityBound(kdt->getComplexityBound() + input->scroll_count);
		ss << kdt->getComplexityBound() + input->scroll_count;
		input->scroll_count = 0;
		console->out(ss.str());


		static size_t last = 12;

		size_t com = kdt->getComplexityBound();
		if (last != com)
		{
			last = com;
			hitpoint_repr1 = nullptr;
			hitpoint_repr1 = nullptr;
			hitpoint_node1 = nullptr;
			hitpoint_node2 = nullptr;
			delete kdt;
			kdt = new KD_Tree(3, shape, com);
		}
	}
	else if (rebuild_kdt)
	{
		rebuild_kdt = false;
		size_t com = kdt->getComplexityBound();
		hitpoint_repr1 = nullptr;
		hitpoint_repr1 = nullptr;
		hitpoint_node1 = nullptr;
		hitpoint_node2 = nullptr;
		delete kdt;
		kdt = new KD_Tree(3, shape, com);
	}

	if (input->is_pressed(GLFW_KEY_R, false))
	{
		//fire a ray
		glm::vec3 collisoin_point;
		Shape* target =  kdt->fireRay(&cam.pos, &cam.front, &collisoin_point, &hitpoint_node1);
		if (target)
		{
			hitpoint_repr1 = new Cube(shader[0], collisoin_point, 0.1, 0.1, 0.1);
			ss << " (" << collisoin_point.x << ", " << collisoin_point.y << ", " << collisoin_point.z << ")";
			if(hitpoint_repr2) ss << " - distance: " << glm::length(hitpoint_repr1->getPosition() - hitpoint_repr2->getPosition());
			console->out(ss.str());
			ss.str("");//reset the stream 

		}
		else
		{
			ss << "RayCast -> no hit detected";
			//delete hitpoint_repr1; CANT DELETE THIS?!?!?!!?	
			hitpoint_repr1 = nullptr;
			console->out(ss.str());
		}
	}else if (input->is_pressed(GLFW_KEY_T, false))
	{
		//fire a ray
		glm::vec3 collisoin_point;
		Shape* target =  kdt->fireRay(&cam.pos, &cam.front, &collisoin_point, &hitpoint_node2);
		if (target)
		{
			hitpoint_repr2 = new Cube(shader[0], collisoin_point, 0.1, 0.1, 0.1);
			ss << " (" << collisoin_point.x << ", " << collisoin_point.y << ", " << collisoin_point.z << ")";
			if (hitpoint_repr1) {
				ss << " - distance: " << glm::length(hitpoint_repr1->getPosition() - hitpoint_repr2->getPosition());
			}
			console->out(ss.str());
			ss.str("");//reset the stream 
		}
		else
		{
			ss << "RayCast -> no hit detected";
			//delete hitpoint_repr1; CANT DELETE THIS?!?!?!!?	
			hitpoint_repr2 = nullptr;
			console->out(ss.str());
		}
	}


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
