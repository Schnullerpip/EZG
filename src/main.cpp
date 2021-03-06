#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//my Includes
#include "Shader.h"
#include "Test_Scene.h"
#include "Light_And_Shadow.h"
#include "Input_Handler.h"
#include "SPG_Scene.h"
#include "DisplacementScene.h"
#include <ctime>


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double, double);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
Input_Handler input(WIDTH, HEIGHT);
EventFeedback feedback;


// The MAIN function, from here we start the application and run the game loop
int main()
{

	srand(time(NULL));

	GLFWwindow* window;
	do
	{
		//(re)set feedback, so feedback loops are prevented
		feedback.reset();
		input.reset();

		// Init GLFW
		glfwInit();
		// Set all the required options for GLFW
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_SAMPLES, feedback.number_samples);

		for (auto p : feedback.hints)
		{
			glHint(p.first, p.second);
		}
		feedback.hints.clear();

		// Create a GLFWwindow object that we can use for GLFW's functions
		window = glfwCreateWindow(WIDTH, HEIGHT, "SPG", nullptr, nullptr);
		glfwMakeContextCurrent(window);

		//soonest possible moment for enabling gl-specifics
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Set the required callback functions
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		// GLFW Options
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
		glewExperimental = GL_TRUE;
		// Initialize GLEW to setup the OpenGL Function pointers
		glewInit();

		// Define the viewport dimensions
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);


		//Scene* scene = new Light_And_Shadow(&input, &feedback);
		Scene* scene = new SPG_Scene(&input, &feedback);
		//Scene* scene = new Test_Scene();
		//Scene* scene = new DisplacementScene(&input, &feedback);

		//Game loop
		//glEnable(GL_CULL_FACE);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		GLfloat last = 0, now = 0, delta = 0;
		while (!feedback.quitgame)
		{
			now = glfwGetTime();
			delta = now - last;

			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene->update(delta, &feedback);
			scene->render(delta);

			// Swap the screen buffers
			glfwSwapBuffers(window);
			last = now;

			//check if the feedback of the update is relevant
			if (feedback.restart)
			{
				break;
			}
		}
		// Terminate GLFW, clearing any resources allocated by GLFW.
		delete scene;
		//glfwTerminate();
		glfwDestroyWindow(window);
	} while (!feedback.quitgame);


	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, GL_TRUE);
	 if (action == GLFW_PRESS)
		input.press_at(key);
	else if (action == GLFW_RELEASE)
		input.release_at(key);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	input.mouse_move(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	input.mouse_scroll(xoffset, yoffset);
}