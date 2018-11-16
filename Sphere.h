#pragma once
#include "Body.h"
class Sphere :
	public Body
{
public:
	Sphere();
	~Sphere();

	void setRadius(float radius) { m_radius = radius; }
	float getRadius() { return m_radius; }
private:
	float m_radius;
};

