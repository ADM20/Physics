#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Particle.h"
#include "Body.h"

// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);
	
	//my transparent shader
	Shader transparent = Shader("resources/shaders/physics.vert", "resources/shaders/physics_trans.frag");
	

	std::vector<Particle> particles;
	int numberOfParticles = 10;

	for (int i = 0; i < numberOfParticles; i++)
	{
		//create particle
		Particle p = Particle::Particle();
		particles.push_back(p);

		particles[i].scale(glm::vec3(.1f, .1f, .1f));
		particles[i].getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));

	}


	//// create particle
	//Mesh particle1 = Mesh::Mesh("resources/models/sphere.obj");
	////scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	//particle1.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	//particle1.scale(glm::vec3(.1f, .1f, .1f));
	//particle1.rotate((GLfloat) M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	//particle1.setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
	
	// create demo objects (a cube and a sphere)
	/*Mesh sphere = Mesh::Mesh("resources/models/sphere.obj");
	sphere.translate(glm::vec3(-1.0f, 1.0f, 0.0f));
	sphere.setShader(lambert);*/
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);

	

	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	
	//fixed timestep
	double physicsTime = 0.0f;
	const double dt = 0.01f;
	double currentTime = (GLfloat)glfwGetTime();
	double accumulator = 0.0f;


	// initialise variables
	glm::vec3 v = glm::vec3(10.0f, 10.0f, 0.0f);
	glm::vec3 a = glm::vec3(0.0f);

	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 f = glm::vec3(0.0f);
	
	glm::vec3 bBox = glm::vec3(5.0f,10.0f,5.0f);
	float damper = 0.8f;


	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		
		
		
		//timestep
		double newTime = (GLfloat)glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;
	

		while (accumulator >= dt)
		{
			for (int i = 0; i < numberOfParticles; i++)
			{	
				/*
				**	SIMULATION
				*/

				//compute forces
				//a = f/M
				//a = (g) / 1.0f;
				particles[i].setAcc(g);

				//update velocity and position
				v = v + deltaTime * a;

				//contact with bounding box

				for (int j = 0; j <3; j++)
				{
					if (particles[i].getPos().y <= 0.0f)
					{
						std::cout << j << std::endl;
						v.y *= (-1.0f * damper);


					}
					else if (particles[i].getPos()[j] >= bBox[j] || particles[i].getPos()[j] <= -5.0f)
					{
						std::cout << j << std::endl;
						v[j] *= (-1.0f * damper);
					}

				}
				particles[i].translate(deltaTime * v);
			}

			accumulator -= dt;
			physicsTime += dt;

		}
		
		// Set frame time
		GLfloat currentFrame = (GLfloat)glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.5f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);

	
		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		for (int i = 0; i < numberOfParticles; i++)
		{
			app.draw(particles[i].getMesh());
		}
	
		// draw demo objects
		app.draw(cube);
		//app.draw(sphere);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

