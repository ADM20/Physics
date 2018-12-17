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
#include "glm/ext.hpp"

// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
#include "RigidBody.h"
#include "Sphere.h"


//time
GLfloat t = 0.0f;
const GLfloat deltaTime = 1.0f / 60.0f;
GLfloat currentTime = (GLfloat)glfwGetTime();
GLfloat accumulator = 0.0f;


//use this to create a random number between a given min and max
float randomGenerator(float min, float max)
{
	float randomNum = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = randomNum * diff;
	return min + r;
}

//main function
int main()
{
	//create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));


	//table mesh
	Mesh poolTable = Mesh::Mesh(Mesh::QUAD);
	//table size x and y
	float tableX = 30;
	float cornerX = poolTable.getPos().x - (tableX / 2.0f);
	float cornerZ = poolTable.getPos().z - (tableX / 2.0f);
	//size of table
	poolTable.scale(glm::vec3(tableX, 0.0f, tableX));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/pool_table.frag");
	poolTable.setShader(lambert);

	//collision grid
	std::vector<Sphere*> cGrid[30][30];
	float cells = 3.0f;
	int gridSize = 3;

	//ball shader
	Shader ballShader = Shader("resources/shaders/ball.vert ", "resources/shaders/ball.frag ");
	Mesh mesh = Mesh::Mesh("resources/models/sphere.obj");

	//make an n number of balls
	const int noBalls = 30;
	Sphere* spheres[noBalls];
	

	//for every ball
	for (int i = 0; i < noBalls; i++)
	{
		//make a temp sphere
		Sphere* s = new Sphere();
		//set up the sphere
		s->setMesh(mesh);
		s->getMesh().setShader(ballShader);
		s->setMass(1.0f);
		//give it a random velocity between two values
		//s->setVel(glm::vec3(randomGenerator(-20, 20), 0.0f, randomGenerator(-20, 20)));
		s->setVel(glm::vec3(0.0f, 0.0f, 0.0f));
		//give it a random position on the table 
		s->setPos(glm::vec3(-10000.0f, -10000.0f, -10000.0f));
		//s->setPos(glm::vec3(1.3*i, s->getRadius(),1.3*i));
		//make the temp sphere the ith ball
		spheres[i] = s;
	}

	//coeficiant of restitution
	float coR = 1.0f;

	//Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//time
		GLfloat newTime = (GLfloat)glfwGetTime();
		GLfloat frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		if (glfwGetKey(app.getWindow(), '0') == GLFW_PRESS)
		{
			for (int i = 0; i < noBalls; i++)
			{
				//make a temp sphere
				Sphere* s = new Sphere();
				//set up the sphere
				s->setMesh(mesh);
				s->getMesh().setShader(ballShader);
				s->setMass(1.0f);
				//give it a random velocity between two values
				s->setVel(glm::vec3(randomGenerator(-20, 20), 0.0f, randomGenerator(-20, 20)));
				//s->setVel(glm::vec3(20.0f, 0.0f, 2.0f));
				//give it a random position on the table 
				s->setPos(glm::vec3(randomGenerator(-tableX / 2.0f, tableX / 2.0f), s->getRadius(), randomGenerator(-tableX / 2.0f, tableX / 2.0f)));
				//s->setPos(glm::vec3(1.3*i, s->getRadius(),1.3*i));
				s->setAngVel(glm::vec3(10.0f, 0.0f, 0.0f));
				std::cout << s->getAngVel().x << std::endl;
				//make the temp sphere the ith ball
				spheres[i] = s;
			}
		}

		app.doMovement(deltaTime);
		//float friction = 1.0f;
		while (accumulator >= deltaTime)
		{

			/*
			**	SIMULATION
			*/

			// for every shpere that exists
			for (Sphere* s : spheres)
			{
				// movement -: translation 
				s->setAcc(s->applyForces(s->getPos(), s->getVel(), t, deltaTime));
				s->setVel(s->getVel() + deltaTime * s->getAcc());
				s->translate(s->getVel() * deltaTime);
				//What cell/s is that ball in?

				//integration rotation
				s->setAngVel(s->getAngVel() + deltaTime * s->getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(s->getAngVel());
				glm::mat3 rotate = glm::mat3(s->getRotate());
				rotate += deltaTime * angVelSkew * rotate;
				rotate = glm::orthonormalize(rotate);
				s->setRotate(rotate);

				//check the x position of the circumferance of the sphere. ->0<-
				int xPlusRad = std::floor((s->getPos().x + s->getRadius() - cornerX) / cells);
				int xMinRad = std::floor((s->getPos().x - s->getRadius() - cornerX) / cells);

				//if ball would have a position off the table, put it back on the table
				if (xPlusRad < 0)
				{
					xPlusRad = 0;
				}
				else if (xPlusRad > gridSize - 1)
				{
					xPlusRad = gridSize - 1;
				}
				if (xMinRad < 0)
				{
					xMinRad = 0;
				}
				else if (xMinRad > gridSize - 1)
				{
					xMinRad = gridSize - 1;
				}
				//An array to hold theses positions
				int x[2] = { xPlusRad, xMinRad };

				//The same but on the Z axis
				int zPlusRad = std::floor((s->getPos().z + s->getRadius() - cornerZ) / cells);
				int zMinRad = std::floor((s->getPos().z - s->getRadius() - cornerZ) / cells);

				//Same check as before except for Z
				if (zPlusRad < 0)
				{
					zPlusRad = 0;
				}
				else if (zPlusRad > gridSize - 1)
				{
					zPlusRad = gridSize - 1;
				}
				if (zMinRad < 0)
				{
					zMinRad = 0;
				}
				else if (zMinRad > gridSize - 1)
				{
					zMinRad = gridSize - 1;
				}
				//the Z position array
				int z[2] = { zPlusRad,zMinRad };


				//update cells with ball positions
				cGrid[x[0]][z[0]].push_back(s);

				//if both Z positions are the same we dont need to update
				if (z[1] != z[0])
				{
					//update Z pos
					cGrid[x[0]][z[1]].push_back(s);
				}
				//if x positions are the same no need to update
				if (x[1] != x[0])
				{
					//update X pos
					cGrid[x[1]][z[0]].push_back(s);

					if (z[1] != z[0])
					{
						//update x and z pos
						cGrid[x[1]][z[1]].push_back(s);
					}
				}


			}

			//Collisions

			//For every position of my grid
			for (int i = 0; i < gridSize; i++)
			{
				for (int j = 0; j < gridSize; j++)
				{
					//for every ball within that grid spot
					for (Sphere* s : cGrid[i][j])
					{


						// Collision with table only if i is 0/max or j is 0/max
						//if this is true then that ball has collided
						if (i == 0 || i == gridSize - 1 || j == 0 || j == gridSize - 1)
						{
							//variables
							bool collision = true; //there was a collision
							glm::vec3 pointOfCollision = s->getPos(); //where did it collide
							glm::vec3 fixTranslation = glm::vec3(0.0f); //to avoid overlap
							glm::vec3 normal = glm::vec3(0.0f); //normal


							//for x,y,z
							for (int i = 0; i < 3; i++)
							{
								//but not y because the ball does not jump
								if (i != 1)
								{
									//check what side of the table it has collided with 
									if (s->getPos()[i] + s->getRadius() >= poolTable.getPos()[i] + tableX)
									{
										//where did it collide?
										pointOfCollision[i] = poolTable.getPos()[i] + tableX;

										//print tests
										//std::cout << poolTable.getPos()[i]+tableX << std::endl;
										//std::cout << s->getPos()[i] + s->getRadius() << std::endl;

										fixTranslation[i] = -((s->getPos()[i] + s->getRadius()) - (poolTable.getPos()[i] + tableX));
										//collision must have been at the edge of the table, not where the ball is currently 

										normal[i] = -1.0f;
										break;
									}
									else if (s->getPos()[i] - s->getRadius() <= poolTable.getPos()[i] - tableX)
									{
										//where did it collide?
										pointOfCollision[i] = poolTable.getPos()[i] - tableX;
										//print tests
										//std::cout << s->getPos()[i]  << std::endl;
										//std::cout << poolTable.getPos()[i] << std::endl;
										//absolute value becuase i dont want to translate negativly
										fixTranslation[i] = glm::abs((poolTable.getPos()[i] - tableX) - (s->getPos()[i] - s->getRadius()));

										normal[i] = 1.0f;
										break;
									}
								}
							}

							//if there was a collision 
							if (collision)
							{
								//translate away from balls

								//variables

								glm::vec3 r = pointOfCollision - s->getPos(); //position vector
								glm::vec3 n = normal;
								glm::vec3 v = s->getVel() + glm::cross(s->getAngVel(), r); // velocity
								//impulse
								float impulse = (-(1.0f + coR) * glm::dot(v, n)) / ((1 / s->getMass()) + glm::dot(n, (glm::cross(s->getInertia()* glm::cross(r, n), r))));

								//correct the translation
								s->translate(fixTranslation);
								// Calculate new velocities
								s->setVel(s->getVel() + (impulse / s->getMass())*n);
							}
						}

						//only do a check for collisions if there are two or more balls within the cell otherwise dont bother trying
						//spheres can't collide with themselves
						if (cGrid[i][j].size() > 1)
						{
							//is this ball
							for (Sphere* collide : cGrid[i][j])
							{
								//colliding with any other ball
								if (s != collide)
								{
									//if they overlap then they are colliding
									if (s->getRadius() + collide->getRadius() > glm::distance(s->getPos(), collide->getPos()))
									{
										//normal
										glm::vec3 n = glm::normalize(collide->getPos() - s->getPos());
										float overlap = (s->getRadius() + collide->getRadius()) - glm::distance(s->getPos(), collide->getPos());
										//velocity
										float v = glm::length(s->getVel() + collide->getVel());
										glm::vec3 translate = -n * (overlap * (glm::length(s->getVel()) / v));
										glm::vec3 fixTranslate = n * (overlap *  (glm::length(collide->getVel()) / v));
										s->translate(translate);
										collide->translate(fixTranslate);
										//relative v
										glm::vec3 vr = collide->getVel() - s->getVel();
										//impulse
										float impulse = (-(1.0f + coR) * glm::dot(vr, n)) / ((1 / s->getMass()) + (1 / collide->getMass()));
										//calculate collision
										s->setVel(s->getVel() - (impulse*n / s->getMass()));
										collide->setVel(collide->getVel() + (impulse*n / collide->getMass()));
									}
								}
							}
						}
					}

					// Clean the cell
					cGrid[i][j].clear();
				}
			}

			accumulator -= deltaTime;
			t += deltaTime;
		}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(poolTable);
		//draw the spheres
		for (Sphere* s : spheres)
		{
			app.draw(s->getMesh());
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}