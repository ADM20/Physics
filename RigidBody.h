#pragma once
#include "Body.h"

class RigidBody :
	public Body
{
public:
	RigidBody();
	~RigidBody();

	//GETTERS AND SETTERS 
	void setAngVel(const glm::vec3 &omega) { m_angVel = omega; }
	void setAngAcc(const glm::vec3 &alpha) { m_angAcc = alpha; }
	void setInertia(const glm::mat3 &invInertia) { m_inertia = invInertia; }
	//when mass is set also set inertia
	void setMass(float mass) { Body::m_mass = mass; Inertia(); }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInertia() { return m_inertia; }
	void Inertia();

private:
	float m_density;
	glm::mat3 m_inertia;
	glm::vec3 m_angVel;
	glm::vec3 m_angAcc;
};