#include "bridgeobstacle.h"
#include "PositionConstraint.h"

BridgeObstacle::BridgeObstacle() : GameObject()
{
	InitialPosition = Vector3(0, 0, 0);
	numLinks = 5;
	sizeMultiplier = 1;
}

void BridgeObstacle::update(float dt)
{

}
