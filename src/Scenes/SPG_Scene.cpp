#include "SPG_Scene.h"
#include "lut.h"
#include "Cube.h"

SPG_Scene::SPG_Scene(Input_Handler* ih, EventFeedback* ef):input(ih)
{
	feedback = ef;
	//start position for camera
	cam.pos = glm::vec3(0, 20, 100);

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
	
	shader.push_back(test);
	shader.push_back(densityShader);
	shader.push_back(createGeometry);
	shader.push_back(renderGeometry);
	shader.push_back(updateParticle);
	shader.push_back(renderParticle);


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

		//test DELETE THIS!!!!!
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
	/*------------------------------------------------------*/



	//generate geometry vertice data
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), 0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof GL_FLOAT));
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
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof GL_FLOAT, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//create the 3D texture
	glGenTextures(1, &tex_3d);
	glBindTexture(GL_TEXTURE_3D, tex_3d);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, xDim_texture3d, yDim_texture3d, zDim_texture3d, 0, GL_RGBA, GL_FLOAT, 0);
		//glTexImage3D(GL_TEXTURE_3D, 0, GL_R16, xDim_texture3d, yDim_texture3d, zDim_texture3d, 0, GL_RED, GL_HALF_FLOAT, 0);
	glBindTexture(GL_TEXTURE_3D, 0);

	//fill the 3D texture with respective call to shader
	glGenFramebuffers(1, &texFrameBuffer); //generate a framebuffer
	glViewport(0, 0, 96, 96);
	glBindFramebuffer(GL_FRAMEBUFFER, texFrameBuffer); //bind to that framebuffer
		densityShader->Use();
		glUniform2fv(glGetUniformLocation(densityShader->Program, "pillars[0]"), 2, glm::value_ptr(pillar[0]));
		glUniform2fv(glGetUniformLocation(densityShader->Program, "pillars[1]"), 2, glm::value_ptr(pillar[1]));
		glUniform2fv(glGetUniformLocation(densityShader->Program, "pillars[2]"), 2, glm::value_ptr(pillar[2]));

		glBindVertexArray(VAO);
				glBindTexture(GL_TEXTURE_3D, tex_3d); //use the density texture
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_3d, 0); //link the framebuffer to a specific texture
					//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
					//glDrawBuffers(1, DrawBuffers);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);
					glReadBuffer(GL_NONE);
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT);
					glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 256); //render call!!! 
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
	glDrawArraysInstanced(GL_POINTS, 0, 96*96*2, 256);
	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	GLuint primitives;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	printf("%u primitives written!\n\n", primitives);

	glDisable(GL_RASTERIZER_DISCARD);
	glFlush();

	glGenVertexArrays(8, &polygonVAO[0]);
	glGenBuffers(8, &polygonVBO[0]);
	for (int i = 0; i < 8; i++)
	{
		GLfloat feedback[177147];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, i * sizeof(feedback), sizeof(feedback), feedback);
		glBindVertexArray(polygonVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, polygonVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(feedback), feedback, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		//geometry.addVertices(feedback); FAILS STACK OVERFLOW
	}
	/*----------------create geometry and read it from transform feedback buffer-----------*/


}


SPG_Scene::~SPG_Scene()
{
}


void SPG_Scene::render(GLfloat deltaTime)
{
	console->update(deltaTime);
	cam.view();
	cam.projection_p(800,600); 

	test->Use();
	glUniform1i(glGetUniformLocation(test->Program, "layer"), input->scroll_count >= 0 ? input->scroll_count : 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_3d);
		cam.apply_to(test);
		glBindVertexArray(VAO);
		if(renderTextureDebug)
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_3D, 0);

	//Shader s("src/Shaders/vertexShader.vs","src/Shaders/simpleFragmentShader.fs","");
	//cam.apply_to(&s);
	//Cube c(&s ,glm::vec3(0, 0, 0), 1, 1, 1);
	//c.draw();


	//render the geometry
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	renderGeometry->Use();
	cam.apply_to(renderGeometry);
	for (int i = 0; i < 8; ++i)
	{
		glBindVertexArray(polygonVAO[i]);
			glDrawArrays(GL_TRIANGLES, 0, 177147);
		glBindVertexArray(0);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	/*---------------------UPDATE PARTICLES---------------------------*/
	glBindVertexArray(particle_vao);
		glEnable(GL_RASTERIZER_DISCARD);//so no fs will be appended to vs->gs->..
			glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[INPUT]); //declare whats the shader input
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_vbo[FEEDBACK_P]); //declare whats the shader position output
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, particle_vbo[FEEDBACK_T]); //declare whats the shader type output
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, particle_vbo[FEEDBACK_L]); //declare whats the shader life output

			//declare which shaders to use
			updateParticle->Use();

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


		//DEBUG DELETE THIS!!!!
		//auto typeToString = [](int t)->char* {
		//		switch (t)
		//		{
		//		case 0:
		//			return "FIRE";
		//		case 1:
		//			return "SMOKE";
		//		case 2:
		//			return "BLEH";
		//		case 3:
		//			return "EMITTER";
		//		default:
		//			return "NOTHING";
		//		}
		//};
		//printf("<%f, %f, %f>", feedback[0], feedback[1], feedback[2]);
		//printf(" --- <%s, %f>\n\n", typeToString(feedback[3]), feedback[4]);

		//overwrite new data into the array_buffer, that is the shaders input
		glBufferData(GL_ARRAY_BUFFER, sizeof(feedback), feedback, GL_STATIC_DRAW);
		glFlush();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glActiveTexture(GL_TEXTURE0);
			texture[0]->use();
			cam.apply_to(renderParticle);
			glDrawArrays(GL_POINTS, 0, particle_num*particle_elements);

		glDisable(GL_BLEND);

		//Shader s("src/Shaders/SPG/Particles/singleParticle.vs", "src/Shaders/simpleFragmentShader.fs", "");
		//cam.apply_to(&s);
		//glDrawArrays(GL_POINTS, 0, particle_num*particle_elements);


		//Shader s("src/Shaders/SPG/Particles/singleParticle.vs","src/Shaders/SPG/Particles/singleParticle.fs","");
		//s.Use();
		//for (int i = 0; i < particle_num*particle_elements; i += 5)
		//{
		//	Cube c(&s ,glm::vec3(0, feedback[i+1], 0), 0.05f, 0.05f, 0.05f);
		//	cam.model(c.getPosition(), c.rotation_angle, c.rotation_axis);
		//	cam.apply_to(&s);
		//	c.draw();
		//}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glBindVertexArray(0);
	/*----------------------------------------------------------------*/
}


GLfloat lastScrollCount = -1;
void SPG_Scene::update(GLfloat deltaTime, EventFeedback* feedback)
{

	if (console->isInInsertMode())return;
	//apply mouse movement to the camera
	cam.update_fps_style(deltaTime, input);

	if (input->is_pressed(GLFW_KEY_TAB, true))
	{
		renderTextureDebug = !renderTextureDebug;
	}

	if (input->is_pressed(GLFW_KEY_SPACE, true))
	{
		for (size_t i = 0; i < particle_num*particle_elements; ++i)
		{
			particle_vertices[i]	= 0;//x,y,z
			particle_vertices[++i]	= 0;
			particle_vertices[++i]	= 0;

			particle_vertices[++i]	= 4;//type
			particle_vertices[++i]	= 0;//lifetime
		}
		//test DELETE THIS!!!!!
		particle_vertices[0] = 10;
		particle_vertices[1] = 0;
		particle_vertices[2] = 0;
		particle_vertices[3] = 3;
		particle_vertices[4] = 666;
		glBindBuffer(GL_ARRAY_BUFFER, particle_vbo[INPUT]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);
	}


	if (lastScrollCount != input->scroll_count)
	{
		lastScrollCount = input->scroll_count;
		std::cout << "layer: " << input->scroll_count << std::endl;
	}
}