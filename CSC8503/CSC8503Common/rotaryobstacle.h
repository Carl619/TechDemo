#pragma once
#include "GameObject.h"
#include "RenderObject.h"
using namespace NCL::CSC8503;

enum RotationAxis {
	X, Y, Z
};
class RotaryObstacle : public GameObject {
public:
	RotaryObstacle() : GameObject() { 
	
		Axis = Y;
	}
	~RotaryObstacle() {

	}
	void update(float dt);
	void setAxis(RotationAxis ax) { Axis = ax; }
private:
	float speed = 100;
	float rotation = 0;
	RotationAxis Axis;
};