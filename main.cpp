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

//*************************
void applyImpulse( RigidBody &rb, glm::vec3 imPos, float impulse, glm::vec3 normal)
{
	//calculate inverse inertia with rotation
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//calculate change in velocity
	glm::vec3 deltaV = (impulse / rb.getMass()) * normal;
	//set new velociy
	rb.setVel(rb.getVel() + deltaV);
	//calculate vector from center of mass to impulse position
	glm::vec3 r = imPos - rb.getPos();
	//calaulte change in angular velocity
	glm::vec3 deltaomega = impulse * inInertia * glm::cross(r, normal);
	//set new angular velocity
	rb.setAngVel(rb.getAngVel() + deltaomega);

}
void applyImpulse(RigidBody &rb, glm::vec3 imPos, glm::vec3 impulse)
{
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//velocity change
	glm::vec3 deltaV = impulse / rb.getMass();
	rb.setVel(rb.getVel() + deltaV);
	glm::vec3 r = imPos - rb.getPos();
	glm::vec3 deltaomega = inInertia * glm::cross(r, impulse);
	rb.setAngVel(rb.getAngVel() + deltaomega);

}
void printVec(glm::vec3 vector)
{
	std::cout << vector.x << ",\t" << vector.y << ",\t" << vector.z << std::endl;
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
	rb.setMass(2.0f);
	rb.setVel(glm::vec3(4.0f, 0.0f, 0.0f));
	//rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.5f));
	rb.setAngVel(glm::vec3(0.1f, 0.1f, 0.1f));

	//add forces - take out for testing impulses
	rb.addForce(gravity);
	//coefficient of restitution
	//rb.setCor(1.0f);
	rb.setCor(0.7f);

	/************************************/
	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//timestep
		double newTime = (GLfloat)glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		//COLLISIONS
		std::vector<glm::vec3> collisions;
		while (accumulator >= dt)
		{
			app.doMovement(dt);
			/*
			**	SIMULATION
			*/
			//calculate inverse inertia with rotation
			glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
			//use this to find the lowest point after the collision
			float lowestY = 0;
			//loop every vertex to check for collisons
			for (int i = 0; i < rb.getMesh().getVertices().size(); i++)
			{
				//cube vector
				glm::vec4 cube = rb.getMesh().getModel() * glm::vec4(glm::vec3(rb.getMesh().getVertices()[i].getCoord()), 1.0f);
				//if point below the plane detect the collision
				if (plane.getPos().y >= cube.y)
				{
					//find the lowest point of collision
					if (plane.getPos().y - cube.y > lowestY)
						lowestY = plane.getPos().y - cube.y;
					collisions.push_back(glm::vec3(cube));
				}
			}
			//if there was a collision
			if (collisions.size() > 0)
			{					
				rb.setPos(1, rb.getPos().y + lowestY);
				glm::vec3 average;

				for (glm::vec3 collision : collisions)
				{
					collision.y += lowestY;
					average += collision;
				}				
				average = average / collisions.size();
				glm::vec3 normal(0.0f,1.0f,0.0f);
				glm::vec3 r = average - rb.getPos();

				glm::vec3 vr = (rb.getVel() + glm::cross(rb.getAngVel(), r));

				//top number
				float topNum = -(1 + rb.getCor()) * glm::dot(vr, normal);
				//denominator
				float denominator = pow(rb.getMass(), -1) + glm::dot(normal, glm::cross(inInertia * glm::cross(r, normal), r));
				//impulse
				float impulse = (topNum / denominator);
				//apply impulse
				applyImpulse(rb, average, impulse, normal);
			}
		
			//if no collision was detected, continue as normal 

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

				//friction, but not at all
				/*if (t >= 4)
				{
					rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
					rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
				}*/			
			//*******************
				accumulator -= dt;
				t += dt;			
		}
		/******************************
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
		//app.draw(cube);
		app.display();
	}
	app.terminate();
	return EXIT_SUCCESS;
}
