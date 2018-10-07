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

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));


	//timestep
	double currentTime = (GLfloat)glfwGetTime();
	double accumulator = 0.0f;
	


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
	int numberOfParticles = 1;

	for (int i = 0; i < numberOfParticles; i++)
	{
		//create particles
		Particle p = Particle::Particle();
		particles.push_back(p);
		std::cout << "made one" << std::endl;
		particles[i].setMesh(Mesh("resources/models/sphere.obj"));
		particles[i].scale(glm::vec3(.1f, .1f, .1f));
		particles[i].getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
		particles[i].setPos(glm::vec3(1.0f, 9.9f,0.0f ));
		particles[i].setVel(glm::vec3(0.0f, 0.0f, 0.0f));
		//particles[i].translate(glm::vec3(i, 0.0f, 0.0f));
	}


	// create particle used as a static measure
	Mesh particle1 = Mesh::Mesh("resources/models/sphere.obj");
	//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	particle1.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	particle1.scale(glm::vec3(.1f, .1f, .1f));
	particle1.rotate((GLfloat)M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	particle1.setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));

	//my cube
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);


	// initialise variables

	//gravity
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);

	//bounding Box
	glm::vec3 bBox = glm::vec3(5.0f, 10.0f, 5.0f);
	//friction damper
	float damper = 0.9f;
	
	//Cone
	glm::vec3 topCone = glm::vec3(0.0f, 4.0f, 0.0f);
	glm::vec3 bottomCone = glm::vec3(0.0f);
	//top and bottom radius
	float topConeR = 2.0f;
	float bottomConeR = 3.0f;
	//Cone total hieght if it went to a point
	float heightCone = (topConeR*(topCone.y - bottomCone.y)) / (topConeR - bottomConeR);
	//Where the cone would be a point if it got that far
	glm::vec3 tip = glm::vec3(topCone);
	tip.y -= heightCone;
	//force
	glm::vec3 forceCone = glm::vec3(0.0f);
	//force coefficient
	float coneForceCo = 20.0f;
	
	
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
				glm::vec3 v = particles[i].getVel();
				glm::vec3 r = particles[i].getPos();
				//compute forces
				particles[i].setAcc(g);


				//particles[i].setVel(particles[i].getVel() + particles[i].getAcc() * dt);
				//particles[i].translate(particles[i].getVel() * dt);

				//crosses with cone
				//height
				if (r.y < topCone.y && r.y >= bottomCone.y)
				{
					//cone at the hieght of the particle
					glm::vec3 heightConeNow = glm::vec3(topCone);
					heightConeNow.y = r.y;
					//radius of cone at height
					float coneR = bottomConeR + (heightConeNow.y / (topCone.y - bottomCone.y))*(topConeR - bottomConeR);
					//distance from center to particle
					float newR = glm::length(heightConeNow - r);
					//radius
					if (newR < coneR)
					{
						//direction of force
						glm::vec3 fdir = r - tip;
						//radius to point at top of cone
						float topR = heightCone * (newR / ((heightCone - (topCone.y - bottomCone.y)) + heightConeNow.y));
						//point at top at new top radius
						glm::vec3 projection = r - topCone;
						projection.y = 0.0f;
						if (projection != glm::vec3(0.0f)) {
							projection = glm::normalize(projection);
						}
						glm::vec3 topPoint = projection * topR;
						topPoint += topCone;
						//vector from point to top of cone through particle
						glm::vec3 dirTop = glm::vec3(topPoint - tip);
						forceCone = ((dirTop - fdir) * (coneR - newR))*coneForceCo;
					}
					else
					{
						forceCone *= 0;
					}
				}
				else
				{
					forceCone *= 0;
				}

				//calculate drag
				//glm::vec3 drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
				//total force
				glm::vec3 F =  g + forceCone;
				//a = F/m
				particles[i].setAcc((F) / particles[i].getMass());
				//semi implicit Eular
				v += dt * particles[i].getAcc();
				r += dt * v;
				//set postition and velocity
				particles[i].setPos(r);
				particles[i].setVel(v);


				//contact with bounding box
				for (int j = 0; j < 3; j++)
				{
					if (particles[i].getPos().y <= 0.0f)
					{
						//std::cout << j << std::endl;
						//FIX THIS
						particles[i].getVel().y *= (-1.0f * damper);


					}
					else if (particles[i].getPos()[j] >= bBox[j] || particles[i].getPos()[j] <= -5.0f)
					{
						//std::cout << j << std::endl;
						//AND THIS
						particles[i].getVel()[j] *= (-1.0f * damper);
					}

				}
				
				//if particle goes over the blow dryers area of influence

				//if (0 < particles[i].getPos().x && particles[i].getPos().x < 2 && 0 < particles[i].getPos().y && particles[i].getPos().y < 4 && -2 < particles[i].getPos().z &&  particles[i].getPos().z < 2)
				//{
				//	//std::cout << particles[i].getPos().x << std::endl;
				//	particles[i].getVel()[1] *= -1.0f;
				//}

				accumulator -= dt;
				t += dt;
			
			}

		}

		//const double alpha = accumulator / dt;

		

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

