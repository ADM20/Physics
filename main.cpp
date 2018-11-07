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
void applyImpulse(RigidBody &rb,glm::vec3 imPos,glm::vec3 impulse )
{
	glm::mat3 inInertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//velocity change
	glm::vec3 dV = impulse / rb.getMass();
	rb.setVel(rb.getVel() + dV);
	glm::vec3 r = imPos - rb.getPos();
	glm::vec3 deltaomega = inInertia * glm::cross(r, impulse);
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


	RigidBody rb = RigidBody::RigidBody();
	Mesh m = Mesh::Mesh(Mesh::MeshType::CUBE);
	rb.setMesh(m);
	rb.getMesh().setShader(lambert);


	// rigid body motion values
	rb.scale(glm::vec3(1.0f,3.0f,1.0f));
	rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	rb.setMass(1.0f);
	rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	
	//add forces - take out for testing impulses
	rb.addForce(gravity);

	//****************
	//IMPULSES
	//position of impulse
	glm::vec3 imPos(0.0f, 5.0f, 0.0f);
	//impulse force
	glm::vec3 impulse(-10.0f, 0.0f, 0.0f);
	//has impulse already happend
	bool applied = false;
	//*************
	//COLLISIONS
	std::vector<glm::vec3> collisions;
	//***************

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

			//****************************
			//inertia
			glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInertia() * glm::mat3(glm::transpose(rb.getRotate()));
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
			//************************
			//collision detection
			for (auto vertex : rb.getMesh().getVertices())
			{
				glm::vec3 coordinates = rb.getMesh().getModel() * glm::vec4(vertex.getCoord(), 1.0f);
				if (coordinates.y <= plane.getPos().y)
				{
					rb.setPos(rb.getPos() + plane.getPos());
					rb.setVel(1, -rb.getVel().y);
					rb.translate(rb.getVel() * dt);
				}
			}
			//****************************
			//impulse after 2s
			if (t >= 2 && !applied)
			{
				applyImpulse( rb,imPos, impulse);
				applied = true;
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
