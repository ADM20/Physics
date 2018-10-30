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
	void setAngAccl(const glm::vec3 &alpha) { M_angAcc = alpha; }
	void setInvInertia(const glm::mat3 &invInertia) { m_invInvertia = invInertia; }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngACC() { return m_angAccl; }
	glm::vec3 getInvInertia() { return m_invInertia; }
	void scale(glm::vec3 vect);

private:
	float m_density;
	glm::mat3 m_invInertia;
	glm::vec3 m_angVel;
	glm::vec3 m_angAccc;
};