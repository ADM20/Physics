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
GLfloat t = 0.0f;
GLfloat dt = 0.01f;
double currentTime = (GLfloat)glfwGetTime();
double accumulator = 0.0f;

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));



	//******** initialise variables *************//

	//gravity
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	//bounding box
	glm::vec3 bBox = glm::vec3(5.0f, 10.0f, 5.0f);
	//friction damper
	float damper = 1.0f;

	//
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	//my transparent shader
	Shader transparent = Shader("resources/shaders/physics.vert", "resources/shaders/physics_trans.frag");

	//multiple particle creator

	std::vector<Particle> particles;
	int numberOfParticles = 2;

	for (int i = 0; i < numberOfParticles; i++)
	{
		//create particles
		Particle p = Particle::Particle();
		particles.push_back(p);
		std::cout << "made one" << std::endl;
		particles[i].setMesh(Mesh("resources/models/sphere.obj"));
		particles[i].scale(glm::vec3(.1f, .1f, .1f));
		particles[i].getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
		particles[i].setPos(glm::vec3(0.0f, 2.0f, 0.0f));
		//particles[i].setVel(glm::vec3(0.0f, 0.0f, 0.0f));
		particles[i].translate(glm::vec3(i*0.2, 0.0f, 0.0f));
		//forces
		Gravity* fgravity = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
		particles[i].addForce(fgravity);
		//Hooke* fsd = new Hooke(&particles[i], &particles[i - 1], 20.0f*i*i, 0.01f, 1.0f);
		//particles[i].addForce(fsd);
	}


	// create particle used as a static measure
	Mesh particle1 = Mesh::Mesh("resources/models/sphere.obj");
	//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	particle1.translate(glm::vec3(0.0f, 2.0f, 0.0f));
	particle1.scale(glm::vec3(.1f, .1f, .1f));
	particle1.rotate((GLfloat)M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	particle1.setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));

	//my cube
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);





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
				
				// Calculate acceleration
				particles[i].setAcc(particles[i].applyForces(particles[i].getPos(), particles[i].getVel(), t, dt));
				// Integrate to calculate new velocity and position
				glm::vec3 v1 = particles[i].getVel();
				particles[i].setVel(particles[i].getVel() + particles[i].getAcc() * dt);
				particles[i].translate(particles[i].getVel() * dt);


				//contact with bounding box
				for (int j = 0; j < 3; j++)
				{
					if (particles[i].getPos().y <= 0.0f)
					{
						//std::cout << i << std::endl;

						particles[i].getVel().y *= (-1.0f * damper);


					}
					else if (particles[i].getPos()[j] >= bBox[j] || particles[i].getPos()[j] <= -5.0f)
					{
						//std::cout << j << std::endl;

						particles[i].getVel()[j] *= (-1.0f * damper);
					}

				}


				accumulator -= dt;
				t += dt;

			}

		}




		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(dt);


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
		app.draw(particle1);
		// draw demo objects
		app.draw(cube);
		//app.draw(sphere);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}
