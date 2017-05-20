#include "DisplacementScene.h"
#include "Cube.h"


DisplacementScene::DisplacementScene(Input_Handler* ih, EventFeedback* ef):input(ih)
{
	feedback = ef;
	cam.pos = glm::vec3(2, 2, -5);
	console = new OnScreenConsole(1.5f, input, feedback, 800, 600);
	input->subscribe(console); //events come from observerpattern

	//shaders
	shader.push_back(new Shader("src/Shaders/Displacement/displacementVertex.vs","src/Shaders/Displacement/displacementFragment.fs", ""));
	shader.push_back(new Shader("src/Shaders/Light_And_Shadow/VertexLight.vs", "src/Shaders/Light_And_Shadow/FragmentLight.fs", ""));

	texture.push_back(new Texture("images/bricksDiffuse.jpg"));
	texture.push_back(new Texture("images/bricksNormal.jpg"));
	texture.push_back(new Texture("images/bricksDisplace.jpg"));

	texture.push_back(new Texture("images/lion.jpg"));
	texture.push_back(new Texture("images/lionNORMAL.png"));
	texture.push_back(new Texture("images/lionDISPLACE.png"));

	shader[0]->Use();
	glUniform1i(glGetUniformLocation(shader[0]->Program, "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shader[0]->Program, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(shader[0]->Program, "depthMap"), 2);
	glUniform1f(glGetUniformLocation(shader[0]->Program, "height_scale"), height_scale);
	glUniform1i(glGetUniformLocation(shader[0]->Program, "parallax"), parallax_mapping);
	glUniform1f(glGetUniformLocation(shader[0]->Program, "numLayers"), numLayers);
	glUniform1f(glGetUniformLocation(shader[0]->Program, "numLayersRefinement"), numLayersRefinement);


	shape.push_back(new Cube(shader[0], glm::vec3(0, 0, -10), 1, 1, 1));
	shape[0]->texture = texture[0];
	shape[0]->normalMap = texture[1];
	shape.push_back(new Cube(shader[0], glm::vec3(2, 0, -10), 1, 1, 1));
	shape[0]->texture = texture[3];
	shape[0]->normalMap = texture[4];

	lightPos = glm::vec3(10.f, 20.f, 0.f);
	light.push_back(new Light(shader[1], lightPos, 0.05f, 0.05f, 0.05f));

	console->registerCommand(&lightFollow, "LIGHT", "resetting the light source");
	console->registerCommand(&adjustCameraSpeed, "CAM", "adjusting camera speed");
	console->registerCommand(&toggleParallax, "TOGGLE", "Parallax occlusion");
	console->registerCommand(&adjustNumLayers, "STEP", "adjusting Step");
	console->registerCommand(&adjustRefinement, "REFINEMENT", "adjusting Refinement Step");
}

void DisplacementScene::render(GLfloat deltaTime)
{
	cam.view();
	cam.projection_p(800,600); 

	cam.apply_to(shader[0]);
	light[0]->apply_to(shader[0]);

	glActiveTexture(GL_TEXTURE0);
	texture[0]->use();
	glActiveTexture(GL_TEXTURE1);
	texture[1]->use();
	glActiveTexture(GL_TEXTURE2);
	texture[2]->use();

	cam.model(shape[0]->getPosition());
	cam.apply_to(shape[0]->shader);
	shape[0]->draw();

	glActiveTexture(GL_TEXTURE0);
	texture[3]->use();
	glActiveTexture(GL_TEXTURE1);
	texture[4]->use();
	glActiveTexture(GL_TEXTURE2);
	texture[5]->use();

	cam.model(shape[1]->getPosition());
	cam.apply_to(shape[1]->shader);
	shape[1]->draw();

	console->update(deltaTime);

	//render light source
	cam.model(light[0]->getPosition());
	cam.apply_to(shader[1]);

	//other uniforms
	light[0]->render();
}

void DisplacementScene::update(GLfloat deltaTime, EventFeedback* feedback)
{
	if (console->isInInsertMode())return;
	cam.update_fps_style(deltaTime, input, cameraSpeed);

	if (lightFollow)
	{
		glm::vec3 posi = cam.pos + (glm::vec3(1, 1, 1) * cam.front);
		light[0]->setPosition(posi);
	}

	if (adjustCameraSpeed)
	{
		static GLfloat lastSpeed = cameraSpeed;
		
		cameraSpeed += input->getScrollDelta();
		if (cameraSpeed < 0) { cameraSpeed = 0; }

		if (lastSpeed != cameraSpeed)
		{
			std::stringstream ss;
			ss << "Adjusted camera speed to: ";
			ss << cameraSpeed;
			console->out(ss.str());
		}
		lastSpeed = cameraSpeed;
	}
	else if (adjustNumLayers)
	{
		static GLfloat lastNumLayers = numLayers;

		numLayers += input->getScrollDelta();
		if (numLayers < 1) { numLayers = 1; }

		shader[0]->Use();
		glUniform1f(glGetUniformLocation(shader[0]->Program, "numLayers"), numLayers);

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

		shader[0]->Use();
		glUniform1f(glGetUniformLocation(shader[0]->Program, "numLayersRefinement"), numLayersRefinement);

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
		shader[0]->Use();
		glUniform1i(glGetUniformLocation(shader[0]->Program, "parallax"), parallax_mapping);
	}
}

DisplacementScene::~DisplacementScene()
{
	delete console;
}
