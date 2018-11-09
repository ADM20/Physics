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
#include "RigidBody.h"

// time
GLfloat t = 0.0f;
GLfloat dt = 0.01f;
double currentTime = (GLfloat)glfwGetTime();
double accumulator = 0.0f;
void outVec3(glm::vec3 v)
{
	std::cout << v.x << ",\t" << v.y << ",\t" << v.z << std::endl;
}
//*************************
void applyImpulse(glm::vec3 impulse, glm::vec3 ipos, RigidBody &rb)
{
<<<<<<< HEAD
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//velocity change
	glm::vec3 dV = impulse / rb.getMass();
	rb.setVel(rb.getVel() + dV);
	glm::vec3 r = imPos - rb.getPos();
	glm::vec3 deltaOmega = inInertia * glm::cross(r, impulse);
	rb.setAngVel(rb.getAngVel() + deltaOmega);
=======
	glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	glm::vec3 deltav = impulse / rb.getMass();
	rb.setVel(rb.getVel() + deltav);
	glm::vec3 r = ipos - rb.getPos();
	glm::vec3 deltaomega = ininertia * glm::cross(r, impulse);
	rb.setAngVel(rb.getAngVel() + deltaomega);
>>>>>>> parent of a983582... Bugfixes

}
//*************

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
	Force* gravity = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);
	Shader pShader = Shader("resources/shaders/physics.vert", "resources/shaders/solid_blue.frag");
	//my transparent shader
	Shader transparent = Shader("resources/shaders/physics.vert", "resources/shaders/physics_trans.frag");
	
	//my cube
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);
	
	Mesh pMesh = Mesh::Mesh("resources/models/sphere.obj");

	//rigid body
	RigidBody rb = RigidBody::RigidBody();
	Mesh m = Mesh::Mesh(Mesh::MeshType::CUBE);
	rb.setMesh(m);
	rb.getMesh().setShader(lambert);
	rb.scale(glm::vec3(1.0f,3.0f,1.0f));
	rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	rb.setMass(1.0f);
	rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	//add forces - take out for testing impulses
	rb.addForce(gravity);

	//****************
	//IMPULSES
	glm::vec3 ipos(0.0f, 5.0f, 0.0f);
	glm::vec3 impulse(-10.0f, 0.0f, 0.0f);
	bool applied = false;
	//***************
<<<<<<< HEAD
	bool ok = true;
=======


>>>>>>> parent of a983582... Bugfixes
	/************************************/
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

<<<<<<< HEAD
			/*
			**	SIMULATION
			*/
			//loop every vertex to check for collisons
			for (int i = 0; i < rb.getMesh().getVertices().size(); i++)
=======
			//****************************
			//inertia
			glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));
			//**********************
			// integration (translation)
			rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel(), t, dt));
			rb.setVel(rb.getVel() + dt * rb.getAcc());
			rb.translate(rb.getVel() * dt);

			//integrateion (rotation)
			rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
			//create skew symmetric matrix for w
			glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
			//create 3x3 rotation matrix from rb rotation matrix
			glm::mat3 R = glm::mat3(rb.getRotate());
			//update rotation matrix
			R += dt * angVelSkew*R;
			R = glm::orthonormalize(R);
			rb.setRotate(glm::mat4(R));
				
			for (auto vertex : rb.getMesh().getVertices())
>>>>>>> parent of a983582... Bugfixes
			{
				glm::vec4 worldspace = rb.getMesh().getModel() * glm::vec4(glm::vec3(rb.getMesh().getVertices()[i].getCoord()), 1.0f);
				//is point below the plane detect the collision
				if (plane.getPos().y >= worldspace.y && ok)
				{
					collisions.push_back(glm::vec3(worldspace));
				}

			}
<<<<<<< HEAD
			//if there was a collision
			if (collisions.size() > 0 && ok)
			{
				//declare average
				glm::vec3 average;
				//loop through collisions
				for (glm::vec3 c : collisions)
				{
					//add collision to average
					average += c;
					//output collision
					outVec3(c);
				}
				//calculate average
				average = average / collisions.size();
				//output average
				std::cout << "AVERAGE" << std::endl;
				outVec3(average);
				//set ok to false
				ok = false;
			}
			//if no collision was detected, continue as normal 
			if (ok)
			{		
				//calculate inverse inetia with rotation
				glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));

				//integration translation
				rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel(), t, dt));
				rb.setVel(rb.getVel() + dt * rb.getAcc());
				rb.translate(rb.getVel() * dt);

				//intergration rotation
				rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
				glm::mat3 R = glm::mat3(rb.getRotate());		
				R += dt * angVelSkew * R;
				R = glm::orthonormalize(R);
				rb.setRotate(R);
=======

			//**************
			//impulse after 2s
			if (t >= 2 && !applied)
			{
				applyImpulse(impulse, ipos, rb);
				applied = true;
>>>>>>> parent of a983582... Bugfixes
			}
			//*******************
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
		// draw Rigid Body
		app.draw(rb.getMesh());
		
		// draw objects
		app.draw(cube);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}
