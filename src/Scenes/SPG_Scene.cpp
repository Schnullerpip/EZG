#include "SPG_Scene.h"
#include "lut.h"
#include "Cube.h"
#include <ctime>
#include <random>

float wind_intensity = 0.2f;
glm::vec3 wind_direction = glm::vec3(1, 0, 0);
bool lightFollow = false;

SPG_Scene::SPG_Scene(Input_Handler* ih, EventFeedback* ef):input(ih)
{
	feedback = ef;
	//start position for camera
	cam.pos = glm::vec3(0, 20, 50);

	console = new OnScreenConsole(1.5f, input, feedback, 800, 600);
	input->subscribe(console); //events come from observerpattern

	//initialize the shaders
	densityShader =  new Shader("src/Shaders/SPG/SPG_vertex.vs", "src/Shaders/SPG/SPG_fragment.fs", "src/Shaders/SPG/SPG_geometry.gs");
	test =			 new Shader("src/Shaders/SPG/debug.vs", "src/Shaders/SPG/debug.fs", "");
	renderGeometry = new Shader("src/Shaders/SPG/renderGeometry.vs", "src/Shaders/SPG/renderGeometry.fs", "");

	createGeometryFeedbackVaryings = new GLchar*[1];
	createGeometryFeedbackVaryings[0] = "positionFeedback";
	createGeometry = new Shader("src/Shaders/SPG/createGeometry.vs",  "src/Shaders/SPG/createGeometry.gs");
	createGeometry->Link(createGeometryFeedbackVaryings, 1);

	GLchar* particleFeedbackVaryings[] = {"positionFeedback", "typeFeedback", "lifeFeedback"};
	updateParticle = new Shader("src/Shaders/SPG/Particles/updateParticle.vs", "src/Shaders/SPG/Particles/updateParticle.gs");
	updateParticle->Link(particleFeedbackVaryings, 3, GL_INTERLEAVED_ATTRIBS);

	renderParticle = new Shader("src/Shaders/SPG/Particles/renderParticle.vs", "src/Shaders/SPG/Particles/renderParticle.fs", "src/Shaders/SPG/Particles/renderParticle.gs");

	texture.push_back(new Texture("images/sparks.png"));
	texture.push_back(new Texture("images/smoke.png"));
	
	shader.push_back(test);
	shader.push_back(densityShader);
	shader.push_back(createGeometry);
	shader.push_back(renderGeometry);
	shader.push_back(updateParticle);
	shader.push_back(renderParticle);

	//shader for the receivers
	Shader* lightAffectedShader = new Shader(
		"src/Shaders/Light_And_Shadow/smartVertex.vs",
		"src/Shaders/Light_And_Shadow/receiverVMS.fs", "");
	lightAffectedShader->Use();
	glUniform1i(glGetUniformLocation(lightAffectedShader->Program, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(lightAffectedShader->Program, "normalMap"), 1);
	glUniform1i(glGetUniformLocation(lightAffectedShader->Program, "depthMap"), 2);
	shader.push_back(lightAffectedShader);

	//shader for the light source
	Shader* lightEmitterShader = new Shader(
		"src/Shaders/Light_And_Shadow/VertexLight.vs",
		"src/Shaders/Light_And_Shadow/FragmentLight.fs", "");
	shader.push_back(lightEmitterShader);

	//lightsources
	light.push_back(new Light(lightEmitterShader, glm::vec3(0, 30, 0), 0.2, 0.2, 0.2));

	texture.push_back(new Texture("images/crate.jpg"));
	texture.push_back(new Texture("images/crate_NRM.png"));

	//primitives
	Cube *floor = new Cube(lightAffectedShader, glm::vec3(0, -10, 0), 200, 1, 200);
	Cube *o1 = new Cube(lightAffectedShader, glm::vec3(0, 10, 20), 1, 1, 10);
	Cube *o2 = new Cube(lightAffectedShader, glm::vec3(-5, 5, -5), 3, 1, 3);

	floor->texture = texture[2];
	o1->texture = texture[2];
	o2->texture = texture[2];

	floor->normalMap = texture[3];
	o1->normalMap = texture[3];
	o2->normalMap = texture[3];

	shape.push_back(floor);
	shape.push_back(o1);
	shape.push_back(o2);


	/*-----------------------PARTICLES----------------------*/
	glGenVertexArrays(1, &particle_vao);
	glBindVertexArray(particle_vao);
		for (size_t i = 0; i < particle_num*particle_elements; ++i)
		{
			particle_vertices[i]	= 0;//x,y,z
			particle_vertices[++i]	= 0;
			particle_vertices[++i]	= 0;

			particle_vertices[++i]	= 4;//type
			particle_vertices[++i]	= 0;//lifetime
		}

		//set initial particle
		particle_vertices[0] = 0;
		particle_vertices[1] = 0;
		particle_vertices[2] = 0;
		particle_vertices[3] = 3;
		particle_vertices[4] = 666;

		glGenBuffers(2, particle_vbo);//in and out
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[INPUT]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);

		//shader input vbo
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)*3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat)*4));

		//shader position output vbo
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[FEEDBACK_P]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), nullptr, GL_STATIC_DRAW);

		//shader type output vbo
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[FEEDBACK_T]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices)/3, nullptr, GL_STATIC_DRAW);

		//shader life output vbo
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[FEEDBACK_L]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices)/3, nullptr, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	generator = std::mt19937(time(nullptr));
	probability = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	/*------------------------------------------------------*/



	//generate geometry vertice data
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//vertice data for creating geometry with marching cubes algorythm
	GLfloat textureVertices[96 * 96 * 2] = {};
	for (int x = 0; x < 96; x++)
	{
		for (int y = 0; y < 96; y++)
		{
			textureVertices[(x * 96 + y) * 2] = x;
			textureVertices[(x * 96 + y) * 2 + 1] = y;
		}
	}
	glGenBuffers(1, &textureVertexBuffer);
	glGenVertexArrays(1, &textureVertexArray);
		glBindVertexArray(textureVertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, textureVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof textureVertices, textureVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//create the 3D texture
	glGenTextures(1, &tex_3d);
	glBindTexture(GL_TEXTURE_3D, tex_3d);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, xDim_texture3d, yDim_texture3d, zDim_texture3d, 0, GL_RGBA, GL_FLOAT, 0);
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_R16, xDim_texture3d, yDim_texture3d, zDim_texture3d, 0, GL_RED, GL_HALF_FLOAT, 0);
	glBindTexture(GL_TEXTURE_3D, 0);

	//fill the 3D texture with respective call to shader
	glGenFramebuffers(1, &texFrameBuffer); //generate a framebuffer
	glViewport(0, 0, 96, 96);
	glBindFramebuffer(GL_FRAMEBUFFER, texFrameBuffer); //bind to that framebuffer
		densityShader->Use();

		glBindVertexArray(VAO);
				glBindTexture(GL_TEXTURE_3D, tex_3d); //use the density texture
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_3d, 0); //link the framebuffer to a specific texture
					//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
					//glDrawBuffers(1, DrawBuffers);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);
					glReadBuffer(GL_NONE);
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					//render call that fills the density map!!! 
					glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 256);
				glBindTexture(GL_TEXTURE_3D, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glViewport(0, 0, 800, 600);
	/*----------------create geometry and read it from transform feedback buffer-----------*/
	glBindVertexArray(textureVertexArray);
	GLuint polygonFeedbackBuffer;
	glGenBuffers(1, &polygonFeedbackBuffer);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, polygonFeedbackBuffer);
	glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 95*95*255*3 /*length x width x height x xyz*/, nullptr, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, polygonFeedbackBuffer);

	//perform feedback transform
	glEnable(GL_RASTERIZER_DISCARD);
	createGeometry->Use();

	//Allocate storage for the UBO (LUT)
	glGenBuffers(1, &lutEdges);
	glBindBuffer(GL_UNIFORM_BUFFER, lutEdges);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(triTable), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(triTable), &triTable);
	GLuint lut_poly_index = glGetUniformBlockIndex(createGeometry->Program, "lut_edges");
	glUniformBlockBinding(createGeometry->Program, lut_poly_index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lutEdges);
	GLint blocksize;
	glGetActiveUniformBlockiv(createGeometry->Program, lut_poly_index, GL_UNIFORM_BLOCK_DATA_SIZE, &blocksize);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_3d);
	glUniform1i(glGetUniformLocation(createGeometry->Program, "tex3d"), 0);

	//did stuff happen?
	GLuint query;
	glGenQueries(1, &query);
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArraysInstanced(GL_POINTS, 0, 95*95, 255);
	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glFlush();

	GLuint primitives;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	printf("%u primitives written!\n\n", primitives);
	auto generatedVertexCount = primitives * 3;
	printf("consisting of %d triangles\n", generatedVertexCount);
	glDeleteQueries(1, &query);
	glDisable(GL_RASTERIZER_DISCARD);

	//Shape object to save the vertices

	GLuint numBuffers, numFullBuffers;
	numBuffers = numFullBuffers = static_cast<int>(generatedVertexCount / singleBufferLength);
	int missingVertices = generatedVertexCount % singleBufferLength;
	if (missingVertices > 0) ++numBuffers;

	geometry = new marching_geo(numBuffers);
	//glGenVertexArrays(numBuffers, &polygonVAO[0]);
	//glGenBuffers(numBuffers, &polygonVBO[0]);

	GLfloat *feedback;
	int i = 0;
	for (; i < numFullBuffers; i++)
	{
		feedback = new GLfloat[singleBufferLength];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, i * singleBufferLength * sizeof(GLfloat), singleBufferLength * sizeof(GLfloat), feedback);
		geometry->vaoVertexCount[i] = singleBufferLength / 3;
		glBindVertexArray(geometry->polygonVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, geometry->polygonVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, singleBufferLength * sizeof(GLfloat), feedback, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		geometry->addVertices(feedback, feedback+singleBufferLength);
	}
	//consider the rest
	if (missingVertices > 0)
	{
		feedback = new GLfloat[missingVertices];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, i * singleBufferLength * sizeof(GLfloat), missingVertices * sizeof(GLfloat), feedback);
		geometry->vaoVertexCount[i] = missingVertices / 3;
		glBindVertexArray(geometry->polygonVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, geometry->polygonVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, missingVertices * sizeof(GLfloat), feedback, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		geometry->addVertices(feedback, feedback+missingVertices);
		
	}
	geometry->shader = renderGeometry;
	shape.push_back(geometry);

	console->out("initializing kd tree");
	//kdt = new KD_Tree(3, shape, 32);
	console->out("initializing kd tree - DONE");
	/*----------------create geometry and read it from transform feedback buffer-----------*/





	//displacment
	//shaders
	//[4]
	displaced_texture = new Texture("images/bricksDiffuse.jpg");
	texture.push_back(displaced_texture);
	displaced_NM = new Texture("images/bricksNormal.jpg");
	texture.push_back(displaced_NM);
	displaced_HM = new Texture("images/bricksDisplace.jpg");
	texture.push_back(displaced_HM);

	displacementShader = new Shader("src/Shaders/Displacement/displacementVertex.vs",
		"src/Shaders/Displacement/displacementFragment.fs", "");
	shader.push_back(displacementShader);
	displacementShader->Use();
	glUniform1i(glGetUniformLocation(displacementShader->Program, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(displacementShader->Program, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(displacementShader->Program, "depthMap"), 2);
	glUniform1f(glGetUniformLocation(displacementShader->Program, "height_scale"), 0.1f);
	glUniform1i(glGetUniformLocation(displacementShader->Program, "parallax"), true);
	glUniform1f(glGetUniformLocation(displacementShader->Program, "numLayers"), 80.f);
	glUniform1f(glGetUniformLocation(displacementShader->Program, "numLayersRefinement"), 40.f);

	displaced_cube = new Cube(displacementShader, glm::vec3(-10, 30, 10), 1, 1, 1);
	shape.push_back(displaced_cube);
	displaced_cube->texture = texture[4];
	displaced_cube->normalMap = texture[5];

	console->registerCommand(&toggleParallax, "TOGGLE", "Parallax occlusion");
	console->registerCommand(&adjustNumLayers, "STEP", "adjusting Step");
	console->registerCommand(&adjustRefinement, "REFINEMENT", "adjusting Refinement Step");
}


SPG_Scene::~SPG_Scene()
{
}


void SPG_Scene::render(GLfloat deltaTime)
{
	console->update(deltaTime);
	cam.view();
	cam.projection_p(800,600); 

	//render shadows
	GLuint dcm = light[0]->renderShadowVSM(shape, window);

	//render the 3dtexture layerwise (DEBUG tool triggered with TAB in the application)
	test->Use();
	glUniform1i(glGetUniformLocation(test->Program, "layer"), input->scroll_count >= 0 ? input->scroll_count : 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_3d);
		cam.apply_to(test);
		glBindVertexArray(VAO);
			if (renderTextureDebug)
				glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_3D, 0);


	//render the lightsources
	for (auto l : light)
	{
		cam.model_translation(l->getPosition());
		cam.apply_to(l->getShader());
		l->render();
	}

	//render the geometry
	for (auto s : shape)
	{
		cam.model(s->getPosition(), s->rotation_angle, s->rotation_axis);
		cam.apply_to(s->shader);
		light[0]->apply_to(s->shader);
		glActiveTexture(GL_TEXTURE0);
		if(s->texture) s->texture->use();

		glActiveTexture(GL_TEXTURE1);
		if(s->normalMap) s->normalMap->use();

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dcm);
		s->render();
	}

	//delete[] img;

	/*---------------------UPDATE PARTICLES---------------------------*/
	glBindVertexArray(particle_vao);
		glEnable(GL_RASTERIZER_DISCARD);//so no fs will be appended to vs->gs->..
			glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[INPUT]); //declare whats the shader input
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_vbo[FEEDBACK_P]); //declare whats the shader position output
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, particle_vbo[FEEDBACK_T]); //declare whats the shader type output
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, particle_vbo[FEEDBACK_L]); //declare whats the shader life output

			//declare which shaders to use
			updateParticle->Use();

			//random seeds for the positiona
			glm::vec3 rands(probability(generator), probability(generator), probability(generator));
			GLint randLoc = glGetUniformLocation(updateParticle->Program, "random");
			glUniform3f(randLoc, rands.x, rands.y, rands.z);

			//wind vector that should influence the smoke - 4th foat is intensity
			glm::vec4 wind(wind_direction.x, wind_direction.y, wind_direction.z, wind_intensity);
			glUniform4f(glGetUniformLocation(updateParticle->Program, "wind"), wind.x, wind.y, wind.z, wind.w);


			//apply the shaders 
			glBeginTransformFeedback(GL_POINTS);
					glDrawArrays(GL_POINTS, 0, particle_num*particle_elements);
			glEndTransformFeedback();

		glDisable(GL_RASTERIZER_DISCARD);//from here on use fs again (vs->gs->fs)
		glFlush();//wait for opengl instructions to be finished before touching the feedback

		//get data from feedback
		GLfloat feedback[particle_num * particle_elements];
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_vbo[FEEDBACK_P]);
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
		glFlush();

		//overwrite new data into the array_buffer, that is the shaders input
		glBufferData(GL_ARRAY_BUFFER, sizeof(feedback), feedback, GL_STATIC_DRAW);
		glFlush();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			cam.apply_to(renderParticle);

			glActiveTexture(GL_TEXTURE0);
			texture[0]->use();
			glUniform1i(glGetUniformLocation(renderParticle->Program, "fireTex"), 0);

			glActiveTexture(GL_TEXTURE1);
			texture[1]->use();
			glUniform1i(glGetUniformLocation(renderParticle->Program, "smokeTex"), 1);


			glDrawArrays(GL_POINTS, 0, particle_num*particle_elements);

		glDisable(GL_BLEND);
		glFlush();


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glBindVertexArray(0);
	/*----------------------------------------------------------------*/


	//displacement
	cam.apply_to(displacementShader);
	light[0]->apply_to(displacementShader);

	glActiveTexture(GL_TEXTURE0);
	displaced_texture->use();
	glActiveTexture(GL_TEXTURE1);
	displaced_NM->use();
	glActiveTexture(GL_TEXTURE2);
	displaced_HM->use();

	cam.model(displaced_cube->getPosition());
	cam.apply_to(displacementShader);
	displaced_cube->render();

	console->update(deltaTime);
}


GLfloat lastScrollCount = -1;
void SPG_Scene::update(GLfloat deltaTime, EventFeedback* feedback)
{

	if (console->isInInsertMode())return;
	//apply mouse movement to the camera
	static float *camSpeed = &cam.camSpeed;
	static float originalSpeed = *camSpeed;
	if (lightFollow)
	{
		static float speed = 4.f;
		glm::vec3 posi = light[0]->getPosition();
		light[0]->setPosition(cam.pos + cam.front * 10.f);
		if (input->is_pressed(GLFW_KEY_W, false))
			posi.z -= speed;
		if (input->is_pressed(GLFW_KEY_S, false))
			posi.z += speed;
		if (input->is_pressed(GLFW_KEY_A, false))
				posi.x -= speed;
		if (input->is_pressed(GLFW_KEY_D, false))
				posi.x += speed;
		light[0]->setPosition(posi);
	}
	else
	{
		cam.update_fps_style(deltaTime, input);
	}

	if (input->is_pressed(GLFW_KEY_TAB, true))
	{
		renderTextureDebug = !renderTextureDebug;
	}
	if (input->is_pressed(GLFW_KEY_LEFT_SHIFT, false))
	{
		*camSpeed = originalSpeed + 40.f;
	}
	else
	{
		*camSpeed = originalSpeed;
	}
	if (input->is_pressed(GLFW_KEY_L))
	{
		lightFollow = !lightFollow;
	}


	if (input->is_pressed(GLFW_KEY_SPACE, true))
	{

		glm::vec3 collision_point;
		Node* n;
		kdt->fireRay(&cam.pos, &cam.front, &collision_point, &n);

		for (size_t i = 0; i < particle_num*particle_elements; ++i)
		{
			particle_vertices[i]	= 0;//x,y,z
			particle_vertices[++i]	= 0;
			particle_vertices[++i]	= 0;

			particle_vertices[++i]	= 4;//type
			particle_vertices[++i]	= 0;//lifetime
		}
		//test DELETE THIS!!!!!
		particle_vertices[0] = collision_point.x;
		particle_vertices[1] = collision_point.y;
		particle_vertices[2] = collision_point.z;
		particle_vertices[3] = 3;
		particle_vertices[4] = 666;
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[INPUT]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);

	}

	if (input->is_pressed(GLFW_KEY_J))
	{
		wind_direction = -cam.front;
	}
	else if (input->is_pressed(GLFW_KEY_N, false))
	{
		wind_intensity += 0.01f;
	}
	else if (input->is_pressed(GLFW_KEY_M, false))
	{
		wind_intensity -= 0.01f;
	}


	if (lastScrollCount != input->scroll_count)
	{
		lastScrollCount = input->scroll_count;
		std::cout << "layer: " << input->scroll_count << std::endl;
	}
	
	//displacement options
	if (adjustNumLayers)
	{
		static GLfloat lastNumLayers = numLayers;

		numLayers += input->getScrollDelta();
		if (numLayers < 1) { numLayers = 1; }

		displacementShader->Use();
		glUniform1f(glGetUniformLocation(displacementShader->Program, "numLayers"), numLayers);

		if (lastNumLayers != numLayers)
		{
			std::stringstream ss;
			ss << "Adjusted NUM layers to: ";
			ss << numLayers;
			console->out(ss.str());
		}
		lastNumLayers = numLayers;
	}
	if (adjustRefinement)
	{
		static GLfloat lastNumLayers = numLayers;

		numLayersRefinement += input->getScrollDelta();
		if (numLayersRefinement < 1) { numLayersRefinement = 1; }

		displacementShader->Use();
		glUniform1f(glGetUniformLocation(displacementShader->Program, "numLayersRefinement"), numLayersRefinement);

		if (lastNumLayers != numLayersRefinement)
		{
			std::stringstream ss;
			ss << "Adjusted Refinement Step to: ";
			ss << numLayersRefinement;
			console->out(ss.str());
		}
		lastNumLayers = numLayersRefinement;
	}

	if (toggleParallax)
	{
		toggleParallax = false;
		parallax_mapping = !parallax_mapping;
		displacementShader->Use();
		glUniform1i(glGetUniformLocation(displacementShader->Program, "parallax"), parallax_mapping);
	}

}