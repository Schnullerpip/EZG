#include "SPG_Scene.h"
#include "lut.h"


SPG_Scene::SPG_Scene(Input_Handler* ih, EventFeedback* ef):input(ih)
{
	feedback = ef;
	//start position for camera
	cam.pos = glm::vec3(0, 0, 40);

	console = new OnScreenConsole(1.5f, input, feedback, 800, 600);
	input->subscribe(console); //events come from observerpattern

	//initialize the shaders
	densityShader =  new Shader("src/Shaders/SPG/SPG_vertex.vs", "src/Shaders/SPG/SPG_fragment.fs", "src/Shaders/SPG/SPG_geometry.gs");
	test =			 new Shader("src/Shaders/SPG/debug.vs", "src/Shaders/SPG/debug.fs", "");
	createGeometry = new Shader("src/Shaders/SPG/createGeometry.vs",  "src/Shaders/SPG/createGeometry.gs");
	renderGeometry = new Shader("src/Shaders/SPG/renderGeometry.vs", "src/Shaders/SPG/renderGeometry.fs", "");
	
	shader.push_back(test);
	shader.push_back(densityShader);
	shader.push_back(createGeometry);
	shader.push_back(renderGeometry);

	//generate vertice data
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
	createGeometry->Link("positionFeedback");
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
	//GLuint query;
	//glGenQueries(1, &query);
	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

	glBeginTransformFeedback(GL_TRIANGLES);
	glDrawArraysInstanced(GL_POINTS, 0, 96*96*2, 256);
	glEndTransformFeedback();

	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	//GLuint primitives;
	//glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	//printf("%u primitives written!\n\n", primitives);

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


	if (lastScrollCount != input->scroll_count)
	{
		lastScrollCount = input->scroll_count;
		std::cout << "layer: " << input->scroll_count << std::endl;
	}
}