# include <iostream>
# include <cmath>
# include "Force.h"
# include "Body.h"
# include "glm/ext.hpp "

glm::vec3 Force::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return glm::vec3(0.0f);
}

/*
**GRAVITY
*/
glm::vec3 Gravity::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	// gravity force
	return mass * glm::vec3(0.0f, -9.8f, 0.0f);

}

/*
** DRAG
*/
glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	// return drag
	glm::vec3 drag = 0.5 * 1.225 * -vel * glm::length(vel) * 1.05 * 0.01;
	return drag;
}

// HOOKE
glm::vec3 Hooke::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	// Transform distances and velocities from 3D to 1D
	float length = glm::length(getParticle2()->getPos() - getParticle1()->getPos());
	glm::vec3 e = (getParticle2()->getPos() - getParticle1()->getPos()) / length;
	float v1 = glm::dot(e, getParticle1()->getVel());
	float v2 = glm::dot(e, getParticle2()->getVel());
	// Compute 1D force
	float fsd = -getStiffnes()*(getRestLength() - length) - getDamperCoefficient()*(v1 - v2);
	// Return the force in 3D
	return fsd * e;
}