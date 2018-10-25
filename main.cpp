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
#include "Force.h"

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


	//bounding box
	glm::vec3 bBox = glm::vec3(5.0f, 10.0f, 5.0f);

	//
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);
	Shader particleShader = Shader("resources/shaders/physics.vert", "resources/shaders/solid_blue.frag");
	//my transparent shader
	Shader transparent = Shader("resources/shaders/physics.vert", "resources/shaders/physics_trans.frag");
	
	//my cube
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);
	
	Mesh pMesh = Mesh::Mesh("resources/models/sphere.obj");
	//create particles
	
	int particleNum = 10;
	std::vector<Particle> p(particleNum);
	float friction = 0.9f;
	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
	float stiffness = 10.0f;
	float damper = 1.0f;
	glm::vec3 pScale = glm::vec3(0.1f, 0.1f, 0.1f);

	p[0] = Particle::Particle();
	p[0].setMesh(pMesh);
	p[0].scale(pScale);
	//p[0].addForce(g);
	//p[0].setVel(glm::vec3(5.0f, 0.0f, 0.0f));
	p[0].getMesh().setShader(particleShader);
	p[0].setPos(glm::vec3(-5.0f, 5.0f, 0.0f));
	
	
	p[particleNum-1] = Particle::Particle();
	p[particleNum-1].setMesh(pMesh);
	p[particleNum-1].scale(pScale);
	//p[particleNum-1].addForce(g);
	p[particleNum-1].getMesh().setShader(particleShader);
	p[particleNum-1].setPos(glm::vec3(5.0f, 5.0f, 0.0f));
	
	for (int i = 1; i < particleNum - 1; i++)
	{
		std::cout << "Made One" << std::endl;
		p[i] = Particle::Particle();
		p[i].setMass(0.1f);
		p[i].setMesh(pMesh);
		p[i].scale(pScale);
		p[i].getMesh().setShader(particleShader);
		p[i].setPos(glm::vec3(p[0].getPos().x + i, p[0].getPos().y, p[0].getPos().z));
		p[i].addForce(g);
		p[i].addForce(new Drag());
		if (i != particleNum - 1)
		{
			p[i].addForce(new Hooke(&p[i], &p[i + 1], stiffness, damper, 1.0f));
		}
		p[i].addForce(new Hooke(&p[i], &p[i - 1], stiffness, damper, 1.0f));
		//p[i].setVel(p[i].getVel * friction);
	}

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
			app.doMovement(dt);

			for (int i = 0; i < particleNum; i++)
			{			
				glm::vec3 v = p[i].getVel();
				glm::vec3 r = p[i].getPos();

				//force
				glm::vec3 F = p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt);
				// acceleration
				p[i].setAcc(F);
				//semi implicit Eular
				v += dt * p[i].getAcc();
				r = dt * v;
				//set postition and velocity
				p[i].translate(r);
				p[i].setVel(v);

				//Less awful Collision Detection using plane intersection
				/*if (p[i].getPos().y <= plane.getPos().y)
				{
					p[i].setPos(1, plane.getPos().y);
					p[i].setVel(1, -p[i].getVel().y * friction );
				}*/

				for (int j = 0; j < 3; j++)
				{
					if (p[i].getPos().y <= plane.getPos().y)
					{
						p[i].setPos(1, plane.getPos().y);
						p[i].getVel()[j] *= (-1.0f * friction);
						p[i].getVel()[0] *= -1.0f;

					}
					else if (p[i].getPos()[j] >= bBox[j] || p[i].getPos()[j] <= -5.0f)
					{
						p[i].getVel()[j] *= (-1.0f * friction);
					}
				}
			}		

			



				accumulator -= dt;
				t += dt;			
		}

		
		

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		for (int i = 0; i < particleNum; i++)
		{
			//std::cout << "DRAW" << std::endl;
			app.draw(p[i].getMesh());
		}
		// draw objects
		app.draw(cube);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}
