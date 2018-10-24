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

	
	
	
	

	// TASK 2.2 HOOKE'S LAW IMPLEMENTATION VARIABLES
	Particle particle2 = Particle::Particle();
	particle2.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	particle2.getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
	Particle particle1 = Particle::Particle();
	particle1.translate(glm::vec3(0.0f, 4.0f, 0.0f));
	particle1.getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
	particle1.addForce(&Gravity::Gravity(glm::vec3(0.0f, -9.8f, 0.0f)));
	Hooke fsd = Hooke::Hooke(&particle1, &particle2, 10.0f, 0.1f, 1.0f);
	particle1.addForce(&fsd);

	
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
				// TASK 2.2 HOOKE'S LAW IMPLEMENTATION
			// Calculate acceleration
			particle1.setAcc(particle1.applyForces(particle1.getPos(), particle1.getVel(), t, dt));
			// Integrate to calculate new velocity and position
			particle1.setVel(particle1.getVel() + particle1.getAcc() * dt);
			particle1.translate(particle1.getVel() * dt);

	


				accumulator -= dt;
				t += dt;

			//}

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
		/*for (int i = 0; i < numberOfParticles; i++)
		{
			app.draw(particles[i].getMesh());
		}*/

		// TASK 2.2 HOOKE'S LAW IMPLEMENTATION DRAW
		app.draw(particle2.getMesh());
		app.draw(particle1.getMesh());

		// draw demo objects
		app.draw(cube);
		//app.draw(sphere);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}
