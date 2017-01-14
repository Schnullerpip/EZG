#include "Light_And_Shadow.h"
#include "Cube.h"
#include <glm/gtx/quaternion.hpp>
#include "Text.h"
#include <stack>
#include "FileIn.h"
#include <chrono>


bool light_follow = false;
bool increase_normal_effect = false;
bool decrease_normal_effect = false;
GLfloat bump_factor = 1.f;
int maxd = 0;

std::vector<Shape*> createDog(glm::vec3 at, Shader* fur, Texture* tex, Texture* nrm, Texture* soft, Texture* soft_nrm);

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

	texture.push_back(new Texture("images/fur.jpg"));
	texture.push_back(new Texture("images/fur_NRM.png"));

	texture.push_back(new Texture("images/soft.jpg"));
	texture.push_back(new Texture("images/fur_NRM.png"));

	Cube* floor =		new Cube(shader[3], glm::vec3(0, -4, 0), 80.f, 2.f, 50.f);
	Cube* highest_cube =new Cube(shader[3], glm::vec3(0,0,-10), 1, 1, 1);
	Cube* cube2 =		new Cube(shader[3], glm::vec3(10,0,-10), 1, 2, 1);
	Cube* cube3 =		new Cube(shader[3], glm::vec3(12,5,-10), 1, 1, 1);
	Cube* cube4 = new Cube(shader[3], glm::vec3(14, 10, -10), 1, 2, 1);
	Cube* wall =		new Cube(shader[3], glm::vec3(-10,0,0), 0.4f, 8, 25); wall->rotation_angle = 45; wall->rotation_axis = glm::vec3(0, 0, 1);
	Cube* unit_cube =	new Cube(shader[3], glm::vec3(10,-2,4), 1, 1, 1);
	Cube* cube5 =		new Cube(shader[3], glm::vec3(-5,10,5), 0.5, 0.5, 0.5);
	Cube* cube6 =		new Cube(shader[3], glm::vec3(-5, 20, 30), glm::vec3(-0.5f, 0.5f, 0.5f));
	Cube* cube7 =		new Cube(shader[3], glm::vec3(2,55,5), 0.5, 0.5, 0.5);
	Cube* floor_in_the_sky =new Cube(shader[3], glm::vec3(2,50,5), 10, 0.5, 10);
	Cube* wall_in_the_sky =new Cube(shader[3], glm::vec3(-20,30,0), 1, 20, 10);


	floor->name = "floor";
	highest_cube->name = "highest_cube";
	cube2->name = "cube2";
	cube3->name = "cube3";
	cube4->name = "cube4";
	wall->name = "wall";
	unit_cube->name = "unit_cube";
	cube5->name = "cube5";
	cube6->name = "cube6";
	cube7->name = "cube7";
	floor_in_the_sky->name = "floor_in_the_sky";
	wall_in_the_sky->name = "wall_in_the_sky";

	//objects
	shape.push_back(floor);
	shape.push_back(highest_cube);
	shape.push_back(cube2);
	shape.push_back(cube3);
	shape.push_back(cube4);
	shape.push_back(wall);
	shape.push_back(unit_cube);
	shape.push_back(cube5);
	shape.push_back(cube6);
	shape.push_back(cube7);
	shape.push_back(floor_in_the_sky);
	shape.push_back(wall_in_the_sky);


	//lights
	light.push_back(new Light(shader[1], glm::vec3(6, 5.5, 5), 0.2, 0.2, 0.2));

	//start position for camera
	cam.pos = glm::vec3(5, 20, 50);

	floor->texture = texture[4];
	highest_cube->texture = texture[2];
	cube2->texture = texture[2];
	wall->texture = texture[0];
	unit_cube->texture = texture[0];
	cube5->texture = texture[2];
	cube3->texture = texture[0];
	cube4->texture = texture[3];
	cube6->texture = texture[0];
	cube7->texture = texture[0];
	floor_in_the_sky->texture = texture[3];
	wall_in_the_sky->texture = texture[0];

	floor->normalMap = texture[5];
	highest_cube->normalMap = texture[3];
	cube2->normalMap = texture[3];
	cube3->normalMap = texture[1];
	cube4->normalMap = texture[3];
	wall->normalMap = texture[1];
	unit_cube->normalMap = texture[1];
	cube5->normalMap = texture[3];
	cube6->normalMap = texture[1];
	cube7->normalMap = texture[1];
	floor_in_the_sky->normalMap = texture[3];
	wall_in_the_sky->normalMap = texture[1];


	//initialize the OnScreenConsole
	console = new OnScreenConsole(1.5f, input, feedback, 800, 600);
	input->subscribe(console); //events come from observerpattern
	std::stringstream ss;
	ss << "MSAA -> using " << feedback->number_samples << " samples";
	console->out(new OnScreenMessage(ss.str()));
	console->registerCommand(&draw_kd_tree, "DRAW KD", "rendering Kd Tree");
	console->registerCommand(&adjust_maxd, "DP", "adjusting ray reflection depth by mouse scroll");
	console->registerCommand(&kd_set_complexity, "COMPLEXITY", "updating KD_TREE's complexity by mouse scroll");
	console->registerCommand(&rebuild_kdt, "REBUILD", "Rebuilding kd tree");
	console->registerCommand(&light_follow, "LIGHT FOLLOW", "following the point light");
	console->registerCommand(&ray_paris, "RAY PARIS", "attempting to fire Ray at Paris KDT");
	console->registerCommand(&kbi, "KBI", "camera is now driven by Kochanek Bartels interpolation");

	console->registerFunction("LOAD DOG",  &dog_arguments, "loading Dog Mesh into the scene - you should rebuild the kdt!");
	console->registerFunction("LOAD PARIS",  &paris_arguments, "loading Paris OBJ in seperate KDT");

	//initialize the KD_Tree
	kdt = new KD_Tree(3, shape);
	bbox_representation = new Cube(shader[1], glm::vec3(), 1, 1, 1);
	ss.clear();
	ss << "[Scene]::Initialized KD_Tree -> size: " << kdt->Size();
	console->out(ss.str());

	//initialize KB_Interpolation
	std::vector<glm::vec3> checkpoints = {
		glm::vec3(-10.f, 0.f, 80.f),
		glm::vec3(-10.f, 0.f, 50.f),
		glm::vec3(0.f, 5.f, 20.f),
		glm::vec3(-3.f, 10.f, 0.f),
		glm::vec3(0.f, 20.f, 0.f),
		glm::vec3(13.f, 10.f, 0.f),
		glm::vec3(5.f, 5.f, 10.f),
		glm::vec3(0.f, 25.f, 20.f),
		glm::vec3(2.f, 30.f, 20.f),
		glm::vec3(10.f, 33.f, 20.f),
		glm::vec3(8.f, 12.f, 20.f),
		glm::vec3(8.f, 12.f, 40.f),
		glm::vec3(8.f, 12.f, 60.f),
	};
	squad.add_quat(0.0f, vec3(0.f, 0.f, 1.f));
	squad.add_quat(30.f, vec3(1.f, 0.f, 1.f));
	squad.add_quat(20.f, vec3(0.f, 1.f, 0.f));
	squad.add_quat(50.f, vec3(0.f, 1.f, 0.f));
	squad.add_quat(10.f, vec3(0.f, 1.f, 1.f));
	squad.add_quat(25.f, vec3(1.f, 0.f, 0.f));
	squad.add_quat(40.f, vec3(1.f, 0.f, 0.f));
	squad.add_quat(50.f, vec3(1.f, 0.f, 0.f));
	squad.add_quat(60.f, vec3(1.f, 0.f, 0.f));
	squad.add_quat(50.f, vec3(0.f, 0.f, 1.f));
	squad.add_quat(120.f, vec3(0.f, 0.f, 1.f));
	squad.add_quat(200.f, vec3(0.f, 0.f, 1.f));
	squad.add_quat(250.f, vec3(0.f, 0.f, 1.f));

	kb = new KB_Interpolation(checkpoints);


}

void Light_And_Shadow::renderKD_Node(Node* node, Shader* s, Camera* cam)
{
	if (!node)return;
	cam->model(node->bbox.getPosition(), 0, glm::vec3(1, 1, 1), glm::vec3(node->bbox.Width(), node->bbox.Height(), node->bbox.Depth()));
	cam->apply_to(bbox_representation->shader);
	float color = node->depth * 0.13f;
	glUniform3f(glGetUniformLocation(bbox_representation->shader->Program, "lightColor"), color, color, color);
	bbox_representation->draw();

	renderKD_Node(node->left(), s, cam);
	renderKD_Node(node->right(), s, cam);
}
void Light_And_Shadow::renderKD(Node* node, Shader* s, Camera* cam, int max_depth, Node* hit_node)
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
			cam->model(n->bbox.getPosition(), 0, glm::vec3(1, 1, 1), glm::vec3(n->bbox.Width(), n->bbox.Height(), n->bbox.Depth()));
			cam->apply_to(bbox_representation->shader);
			float color = n->depth * 0.13f;
			glUniform3f(glGetUniformLocation(bbox_representation->shader->Program, "lightColor"), color, color, color);
			bbox_representation->draw();
			nodes.pop();
		}
	else
		renderKD_Node(node, s, cam);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void Light_And_Shadow::render(GLfloat deltaTime) {

	//first render shadow
	light[0]->renderShadow(shape, window);

	if (kbi)
	{
		glm::vec3 cP = kb->step();
		glm::mat4 view_factor;
		view_factor = glm::toMat4(squad.step(kb->getP(), kb->getT()));
		cam.view(glm::vec3(cP.x, cP.y, cP.z + 10), cam.front , cam.up, view_factor);
		cam.projection_p(800, 600, 0.1f, 300.f);
	}
	else
	{
		cam.view();
		cam.projection_p(800,600); 
	}


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
		cam.model(shape[i]->getPosition(), shape[i]->rotation_angle, shape[i]->rotation_axis);
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

		if (hitpoint_repr1 && hitpoint_repr2) {
			cam.model_tmp = glm::mat4();
			cam.apply_to(shader[2]);
			Shape::drawLine(hitpoint_repr1->getPosition(), hitpoint_repr2->getPosition(), shader[2], 4);
		}
	}

	console->update(deltaTime);
}

void Light_And_Shadow::update(GLfloat deltaTime, EventFeedback* feedback) {
	if (console->isInInsertMode())return;
	std::string message;
	std::stringstream ss;

	if(input->is_pressed(GLFW_KEY_SPACE, false))
		light_follow = !light_follow;

	increase_normal_effect = input->is_pressed(GLFW_KEY_K, false);
	decrease_normal_effect = input->is_pressed(GLFW_KEY_J, false) && !increase_normal_effect;

	static size_t last = 12;
	static int last_depth = 0;

	if (!dog_arguments.empty())
	{
		std::stringstream ss(dog_arguments);
		glm::vec3 dogPosition;
		ss >> dogPosition.x;
		ss >> dogPosition.y;
		ss >> dogPosition.z;
		std::vector<Shape*> dog = createDog(dogPosition, shader[3], texture[6], texture[7], texture[8], texture[9]);
		shape.insert(shape.end(), dog.begin(), dog.end());
		dog_arguments = "";
	}
	else if (ray_paris)
	{
		ray_paris = false;

		if (!paris_kdt)
		{
			console->out("Please first load a Paris model in, by typing in $>LOAD PARIS [complexity]");
			return;
		}

		glm::vec3 collision_point;
		Node* n;

		ss.str("");
		auto start = std::chrono::high_resolution_clock::now();
		if (paris_kdt->fireRay(&cam.pos, &cam.front, &collision_point, &n))
		{
			auto end = std::chrono::high_resolution_clock::now();
			ss << "Hit Paris model at (" << collision_point.x << ", " << collision_point.y << ", " << collision_point.z << ") - took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns";
		}
		else
		{
			ss << "Did not hit Paris model";
		}
		console->out(ss.str());
	}

	if (!paris_arguments.empty())
	{
		static bool step1 = false, step2 = false;

		std::stringstream arg(paris_arguments);
		static int complexity;
		static std::vector<TriangleContainer*> tris;
		static std::ifstream input;

		if (!step1)
		{
			step1 = true;
			arg >> complexity;
			console->out("1. Reading paris.obj from file");
			input.open("objs/Paris2010_Simple.obj");
			return;
		}

		if (!step2)
		{
			step2 = true;
			ReadObjFile(input, tris);
			console->out("2. finished parsing paris.obj");
			console->out("3. Building the KD Tree");
			return;
		}

		if(paris_kdt)
			delete paris_kdt;

		auto start = std::chrono::high_resolution_clock::now();
		paris_kdt = new KD_Tree(3, tris, complexity);
		auto end = std::chrono::high_resolution_clock::now();

		std::stringstream ss;
		ss << "4. Finished building the Paris KD Tree with " << tris.size() << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
		console->out(ss.str());
		ss.str("");
		ss << "Paris KDT - complexity:" << paris_kdt->getComplexityBound();
		console->out(ss.str());
		paris_arguments = "";
		step1 = step2 = false;
	}

	if (adjust_maxd || kd_set_complexity)
	{
		std::stringstream ss;

		if (kd_set_complexity)
		{
			int value = kdt->getComplexityBound() + input->scroll_count;
			if (value > 0)
			{
				kdt->setComplexityBound(value);

				size_t com = kdt->getComplexityBound();
				if (last != com)
				{
					last = com;
					ss << "KDT::Complexity -> " << kdt->getComplexityBound() + input->scroll_count;
					console->out(ss.str());
					ss.str("");
					rebuildKDT();
				}
			}
		}
		if (adjust_maxd)
		{
			int value = maxd + input->scroll_count;
			if (last_depth != value && value > 0)
			{
				last_depth = maxd = value;
				ss << "KDT::RayTrace -> reflection depth -> " << maxd;
				console->out(ss.str());
				ss.str("");
			}

		}

		input->scroll_count = 0;
	}
	if (rebuild_kdt)
	{
		rebuildKDT();
	}

	if (input->is_pressed(GLFW_KEY_R, false))
	{
		//fire a ray
		glm::vec3 collisoin_point;
		Shape* target =  kdt->fireRay(&cam.pos, &cam.front, &collisoin_point, &hitpoint_node1);
		if (target)
		{
			if (hitpoint_repr1) delete hitpoint_repr1;
			hitpoint_repr1 = new Cube(shader[0], collisoin_point, 0.1, 0.1, 0.1);
			ss << " (" << collisoin_point.x << ", " << collisoin_point.y << ", " << collisoin_point.z << ")";
			if(hitpoint_repr2) ss << " - distance: " << glm::length(hitpoint_repr1->getPosition() - hitpoint_repr2->getPosition());
			console->out(ss.str());
			ss.str("");//reset the stream 
		}
		else
		{
			ss << "RayCast -> no hit detected";
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
			if (hitpoint_repr2) delete hitpoint_repr2;
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

std::vector<Shape*> createDog(glm::vec3 at, Shader* fur, Texture* tex, Texture* nrm, Texture* soft, Texture* soft_nrm)
{
	std::vector<Shape*> dog;
	//dog mesh 
	Cube* nose, *nosePit, *head, *neck, *body, *legFR, *legFL, *legBR, *legBL, *tail, *earL, *earR, *eyeL, *eyeR;
	nose = new Cube(fur, glm::vec3(at.x, at.y-1, at.z + 1), 3.5, 3, 5); nose->rotation_angle = glm::radians(15.f); nose->rotation_axis = glm::vec3(1, 0, 0);
	eyeL = new Cube(fur, glm::vec3(at.x-1, at.y+1, at.z-1), 1, 2, 1); 
	eyeR = new Cube(fur, glm::vec3(at.x+1, at.y+1, at.z-1), 1, 2, 1); 
	nosePit = new Cube(fur, glm::vec3(at.x, at.y-0.5, at.z + 3.5), 2, 1, 1);
	head = new Cube(fur, glm::vec3(at.x, at.y, at.z-4), 6, 5, 6);
	neck = new Cube(fur, glm::vec3(at.x, at.y-4, at.z-7), 3, 9, 3); neck->rotation_angle = 10; neck->rotation_axis = glm::vec3(1, 0, 0);
	body = new Cube(fur, glm::vec3(at.x, at.y-10, at.z-14), 7, 7, 11);body->rotation_angle = glm::radians(45.f); body->rotation_axis = glm::vec3(0, 0, 1);
	legFL =new Cube(fur, glm::vec3(at.x-3, at.y-15, at.z-7), 2, 8, 2);legFL->rotation_angle = glm::radians(-30.f);legFL->rotation_axis = glm::vec3(1, 0, 0);
	legFR =new Cube(fur, glm::vec3(at.x+3, at.y-12.5, at.z-5.5), 2, 8, 2);legFR->rotation_angle = glm::radians(-55.f);legFR->rotation_axis = glm::vec3(1, 0, 0);
	legBL =new Cube(fur, glm::vec3(at.x-3, at.y-15, at.z-22), 2, 8, 2);legBL->rotation_angle = glm::radians(30.f);legBL->rotation_axis = glm::vec3(1, 0, 0);
	legBR =new Cube(fur, glm::vec3(at.x+3, at.y-12.5, at.z-22), 2, 8, 2);legBR->rotation_angle = glm::radians(55.f);legBR->rotation_axis = glm::vec3(1, 0, 0);
	tail = new Cube(fur, glm::vec3(at.x, at.y-4, at.z-20.5), 1, 5, 1);tail->rotation_angle = glm::radians(-35.f);tail->rotation_axis = glm::vec3(1, 0, 0);
	earL = new Cube(fur, glm::vec3(at.x+3, at.y+2.5, at.z - 6), 2, 2, 1); earL->rotation_angle = glm::radians(45.f); earL->rotation_axis = glm::vec3(0, 0, 1);
	earR = new Cube(fur, glm::vec3(at.x-3, at.y+2.5, at.z - 6), 2, 2, 1); earR->rotation_angle = glm::radians(45.f); earR->rotation_axis = glm::vec3(0, 0, 1);

	nose->name = "nose";
	nosePit->name = "nosePit";
	head->name = "head";
	neck->name = "neck";
	body->name = "body";
	legFL->name = "legFL";
	legBL->name = "legBL";
	legBR->name = "BR";
	tail ->name = "tail";
	earL ->name = "earL";
	earR ->name = "earR";

	dog.push_back(nose);
	dog.push_back(eyeL);
	dog.push_back(eyeR);
	dog.push_back(nosePit);
	dog.push_back(head);
	dog.push_back(neck);
	dog.push_back(body);
	dog.push_back(legFL);
	dog.push_back(legFR);
	dog.push_back(legBL);
	dog.push_back(legBR);
	dog.push_back(tail);
	dog.push_back(earL);
	dog.push_back(earR);

	nose->texture = tex;
	nose->normalMap = nrm;
	eyeL->texture = soft;
	eyeL->normalMap = soft_nrm;
	eyeR->texture = soft;
	eyeR->normalMap = soft_nrm;
	nosePit->texture = soft;
	nosePit->normalMap = soft_nrm;
	head->texture = tex;
	head->normalMap = nrm;
	neck->texture = tex;
	neck->normalMap = nrm;
	body->texture = tex;
	body->normalMap = nrm;
	legFL->texture = tex;
	legFL->normalMap = nrm;
	legFR->texture = tex;
	legFR->normalMap = nrm;
	legBL->texture = tex;
	legBL->normalMap = nrm;
	legBR->texture = tex;
	legBR->normalMap = nrm;
	tail->texture = tex;
	tail->normalMap = nrm;
	earL->texture = tex;
	earL->normalMap = nrm;
	earR->texture = tex;
	earR->normalMap = nrm;

	return dog;
}

void Light_And_Shadow::rebuildKDT()
{
	rebuild_kdt = false;
	size_t com = kdt->getComplexityBound();

	if(hitpoint_repr1)
		delete hitpoint_repr1;
	if(hitpoint_repr2)
		delete hitpoint_repr2;

	hitpoint_repr1 = nullptr;
	hitpoint_repr2 = nullptr;
	hitpoint_node1 = nullptr;
	hitpoint_node2 = nullptr;

	delete kdt;
	kdt = new KD_Tree(3, shape, com);
	std::stringstream ss;
	ss << "rebuild the KD Tree -> " << kdt->Size() << " nodes -> " << kdt->Root()->triangles.size() << " triangles";
	console->out(ss.str());
}
