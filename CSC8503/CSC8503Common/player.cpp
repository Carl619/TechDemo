#include "player.h"

void Player::respawn()
{
	if (MustRespawn) {
		transform.SetWorldPosition(OriginalPosition);
		MustRespawn = false;
	}
}

void Player::addHit()
{
	NumberHits++;
}

void Player::update(float dt)
{
}
