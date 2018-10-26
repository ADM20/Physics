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
double time1 = t;//for my random number gen

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
	std::default_random_engine generate;
	std::uniform_real_distribution<float> variable(-10.0f, 10.0f);
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
	
	//**** PARTICLE CHAIN ****
	////create particles
	//int particleNum = 10;
	//std::vector<Particle> p(particleNum);
	float friction = 0.1f;
	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
	float stiffness = 10.0f;
	float damper = 10.0f;
	glm::vec3 pScale = glm::vec3(0.1f, 0.1f, 0.1f);

	//p[0] = Particle::Particle();
	//p[0].setMesh(pMesh);
	//p[0].scale(pScale);
	////p[0].addForce(g);
	////p[0].setVel(glm::vec3(5.0f, 0.0f, 0.0f));
	//p[0].getMesh().setShader(particleShader);
	//p[0].setPos(glm::vec3(-5.0f, 5.0f, 0.0f));
	//
	//
	//p[particleNum-1] = Particle::Particle();
	//p[particleNum-1].setMesh(pMesh);
	//p[particleNum-1].scale(pScale);
	////p[particleNum-1].addForce(g);
	//p[particleNum-1].getMesh().setShader(particleShader);
	//p[particleNum-1].setPos(glm::vec3(5.0f, 5.0f, 0.0f));
	//
	//for (int i = 1; i < particleNum - 1; i++)
	//{
	//	std::cout << "Made One" << std::endl;
	//	p[i] = Particle::Particle();
	//	p[i].setMass(0.1f);
	//	p[i].setMesh(pMesh);
	//	p[i].scale(pScale);
	//	p[i].getMesh().setShader(particleShader);
	//	p[i].setPos(glm::vec3(p[0].getPos().x + i, p[0].getPos().y, p[0].getPos().z));
	//	p[i].addForce(g);
	//	p[i].addForce(new Drag());
	//	if (i != particleNum - 1)
	//	{
	//		p[i].addForce(new Hooke(&p[i], &p[i + 1], stiffness, damper, 1.0f));
	//	}
	//	p[i].addForce(new Hooke(&p[i], &p[i - 1], stiffness, damper, 1.0f));
	//	//p[i].setVel(p[i].getVel * friction);
	//}

	//*** CLOTH ***
	//create particles
	const int particleNum = 10; //create an n x n particle array
	Particle particles[particleNum][particleNum];
	//std::vector<Particle> particles(particleNum);
	for (int i = 0; i < particleNum; i++)
	{
		for (int j = 0; j < particleNum; j++)
		{
			Particle particle = Particle::Particle();
			particle.translate(glm::vec3(-5.0f + i, 10.0f, -2.5f + j));
			//particles[i][j].setMesh(pMesh);
			particle.getMesh().setShader(Shader(particleShader));
			particles[i][j] = particle;
			//add forces
			particles[i][j].addForce(g);
			particles[i][j].addForce(new Drag());
			particles[i][j].addForce(new Hooke(&particles[i][j], &particles[i - 1][j], stiffness, damper, 1.0f));
			particles[i][j].addForce(new Hooke(&particles[i][j], &particles[i + 1][j], stiffness, damper, 1.0f));
			particles[i][j].addForce(new Hooke(&particles[i][j], &particles[i][j - 1], stiffness, damper, 1.0f));
			particles[i][j].addForce(new Hooke(&particles[i][j], &particles[i][j + 1], stiffness, damper, 1.0f));

		}
	}

	//Cone
	
	glm::vec3 topCone = glm::vec3(0.0f,8.0f, 0.0f);
	glm::vec3 bottomCone = glm::vec3(-1.0f);
	//top and bottom radius
	float topConeR = 10.0f;
	float bottomConeR = 5.0f;
	//Cone total hieght if it went to a point
	float heightCone = (topConeR*(topCone.y - bottomCone.y)) / (topConeR - bottomConeR);
	//Where the cone would be a point if it got that far
	glm::vec3 tip = glm::vec3(topCone);
	tip.y -= heightCone;
	//force
	glm::vec3 forceCone = glm::vec3(0.0f);
	//force coefficient
	float coneForceCo = 1.0f;

	
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
			//***** PARTICLE CHAIN *******
			//for (int i = 0; i < particleNum; i++)
			//{			
			//	glm::vec3 v = p[i].getVel();
			//	glm::vec3 r = p[i].getPos();

			//	//force
			//	glm::vec3 F = p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt);
			//	// acceleration
			//	p[i].setAcc(F);
			//	//semi implicit Eular
			//	v += dt * p[i].getAcc();
			//	r = dt * v;
			//	//set postition and velocity
			//	p[i].translate(r);
			//	p[i].setVel(v);

			//	//Less awful Collision Detection using plane intersection
			//	/*if (p[i].getPos().y <= plane.getPos().y)
			//	{
			//		p[i].setPos(1, plane.getPos().y);
			//		p[i].setVel(1, -p[i].getVel().y * friction );
			//	}*/

			//	for (int j = 0; j < 3; j++)
			//	{
			//		if (p[i].getPos().y <= plane.getPos().y)
			//		{
			//			p[i].setPos(1, plane.getPos().y);
			//			p[i].getVel()[j] *= (-1.0f * friction);
			//			p[i].getVel()[0] *= -1.0f;

			//		}
			//		else if (p[i].getPos()[j] >= bBox[j] || p[i].getPos()[j] <= -5.0f)
			//		{
			//			p[i].getVel()[j] *= (-1.0f * friction);
			//		}
			//	}



			//}		
			//loop all my particles
			for (int i = 1; i < particleNum - 1; i++)
			{
				for (int j = 1; j < particleNum - 1; j++)
				{
						glm::vec3 v = particles[i][j].getVel();
						glm::vec3 r = particles[i][j].getPos();

						//force
						glm::vec3 F = particles[i][j].applyForces(particles[i][j].getPos(), particles[i][j].getVel(), t, dt);
						// acceleration
						particles[i][j].setAcc(F);
						//semi implicit Eular
						v += dt * particles[i][j].getAcc();
						r = dt * v;
						//set postition and velocity
						particles[i][j].translate(r);
						particles[i][j].setVel(v);

					// collide with floor
					if (particles[i][j].getPos().y <= plane.getPos().y)
					{
						//stop particles from falling through the floor
						particles[i][j].setPos(1, plane.getPos().y);
						//velocity boys!!
						particles[i][j].setVel(1, -particles[i][j].getVel().y * friction);
						
					}


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
					//total force
					 F += glm::vec3(0.0f,-9.8f,0.0f) + forceCone;
					//a = F/m
					particles[i][j].setAcc((F) / particles[i][j].getMass());
					//semi implicit Eular
					v += dt * particles[i][j].getAcc();
					r += dt * v;
					//set postition and velocity
					//particles[i][j].setPos(r);
					particles[i][j].setVel(v);
				}
			}

			if (t - time1 >= 3)
			{
				float randomNum = (float)(rand() % 10) - 5;
				topCone.z = randomNum;
				time1 = t;
				std::cout << topCone.z << std::endl;
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
		//*** PARTICLE CHAIN *****
		//for (int i = 0; i < particleNum; i++)
		//{
		//	//std::cout << "DRAW" << std::endl;
		//	app.draw(p[i].getMesh());
		//}

		//draw cloth
		for (int i =0; i < particleNum; i++)
		{
			for (int j = 0; j < particleNum; j++)
			{

				app.draw(particles[i][j].getMesh());
			}
		}
		// draw objects
		//app.draw(cube);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}
