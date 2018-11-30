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
#include "Sphere.h"

// time
GLfloat t = 0.0f;
GLfloat dt = 0.0166;
double currentTime = (GLfloat)glfwGetTime();
double accumulator = 0.0f;

//*************************
//float impulse
void applyImpulse(RigidBody &rb, glm::vec3 imPos, float impulse, glm::vec3 normal)
{
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//velocity change
	glm::vec3 deltaV = (impulse / rb.getMass()) * normal;
	rb.setVel(rb.getVel() + deltaV);
	glm::vec3 r = imPos - rb.getPos();
	//angular velocity change
	glm::vec3 deltaomega = impulse * inInertia * glm::cross(r, normal);
	//new angular velocity
	rb.setAngVel(rb.getAngVel() + deltaomega);

}
//vector impulse
void applyImpulse(RigidBody &rb, glm::vec3 imPos, glm::vec3 impulse)
{
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//velocity change
	glm::vec3 deltaV = impulse / rb.getMass();
	rb.setVel(rb.getVel() + deltaV);
	glm::vec3 r = imPos - rb.getPos();
	//angular velocity change
	glm::vec3 deltaomega = inInertia * glm::cross(r, impulse);
	//new angular velocity
	rb.setAngVel(rb.getAngVel() + deltaomega);

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
	//pool table size
	plane.scale(glm::vec3(15.0f, 1.0f, 30.0f));
	Shader poolTable = Shader("resources/shaders/physics.vert", "resources/shaders/pool_table.frag");
	plane.setShader(poolTable);
	Shader pShader = Shader("resources/shaders/physics.vert", "resources/shaders/solid_blue.frag");
	//my transparent shader
	Shader transparent = Shader("resources/shaders/physics.vert", "resources/shaders/physics_trans.frag");

	//my cube
	Mesh cube = Mesh::Mesh("resources/models/cube.obj");
	cube.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	cube.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	cube.setShader(transparent);

	Mesh sphereMesh = Mesh::Mesh("resources/models/sphere.obj");

	////rigid body
	//RigidBody rb = RigidBody::RigidBody();
	//Mesh m = Mesh::Mesh(Mesh::MeshType::CUBE);
	//rb.setMesh(m);
	//rb.getMesh().setShader(poolTable);
	//rb.scale(glm::vec3(1.0f, 3.0f, 1.0f));
	//rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	//rb.setMass(2.0f);
	//rb.setVel(glm::vec3(5.0f, 0.0f, 0.0f));
	////rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.5f));
	//rb.setAngVel(glm::vec3(0.0f, 1.0f, 0.2f));

	////add forces - take out for testing impulses
	//rb.addForce(gravity);
	////coefficient of restitution
	////rb.setCor(1.0f);
	//rb.setCor(0.6f);

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
		//app.draw(rb.getMesh());
		//draw sphere
		//app.draw(sp.getMesh());
		// draw objects
		//app.draw(cube);
		app.display();
	}
	app.terminate();
	return EXIT_SUCCESS;
}
