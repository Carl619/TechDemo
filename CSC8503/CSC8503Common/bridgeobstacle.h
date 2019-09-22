#pragma once
#include "GameObject.h"
#include "RenderObject.h"
using namespace NCL::CSC8503;

class BridgeObstacle : public GameObject {
public:
	BridgeObstacle();
	~BridgeObstacle() {

	}
	void update(float dt);
	void setInitialPosition(Vector3 pos) { InitialPosition = pos; }
	Vector3 getInitialPosition() { return InitialPosition; }

	void setSizeMultiplier(float size) { sizeMultiplier = size; }
	float getSizeMultiplier() { return sizeMultiplier; }

	void setnumLinks(int links) { numLinks = links; }
	int getnumLinks() { return numLinks; }
private:
	
	float sizeMultiplier;
	int numLinks;
	std::vector<GameObject*> piecesBridges;
	Vector3 InitialPosition;
};