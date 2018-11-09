#define _USE_MATH_DEFINES
#include <cmath>
#include "RigidBody.h"


RigidBody::RigidBody()
{
	
}


RigidBody::~RigidBody()
{

}
void RigidBody::Inertia()
{
	//impliment inertia equation for cube
	float massBy12 = this->getMass()/12;

	float wSq = pow(this->getScale()[0][0] * 2, 2);
	float dSq = pow(this->getScale()[2][2] * 2, 2);
	float hSq = pow(this->getScale()[1][1] * 2, 2);

	float first = massBy12 * (hSq + dSq);
	float second = massBy12 * (wSq + dSq);
	float third = massBy12 * (wSq + hSq);
	glm::mat3 inertia = glm::mat3(0.0f);

	inertia[0][0] = first;
	inertia[1][1] = second;
	inertia[2][2] = third;

	this->m_inertia = glm::inverse(inertia);
}