#pragma once
#include "GameObject.h"
#include "RenderObject.h"
using namespace NCL::CSC8503;

class Player : public GameObject {
public:
	Player() : GameObject() { NumberHits = 0; }
	~Player() {

	}
	void respawn();
	void addHit();
	void update(float dt);
	int getHits() { return NumberHits; }
	void setHits(int hits) { NumberHits = hits; }
	void setOriginalPosition(Vector3 pos) { OriginalPosition = pos; }
	void HaveToRespawn() { MustRespawn = true; }
private:
	int NumberHits;
	bool MustRespawn = false;
	Vector3 OriginalPosition;
};