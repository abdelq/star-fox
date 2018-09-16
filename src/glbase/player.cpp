#include "player.h"
#include <glm/ext.hpp>

#define X_AXIS vec3(1.0f,0.0f,0.0f);
Player::Player()
{
	std::srand(std::time(0));

	// Build player's ship (build a hierarchy using primitives and transformations)
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Player::Render()
{
	// Render player's ship
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Player::Update(double dt)
{
	accel = vec2();

	// Modify accel vector according to input[x], with x in {DOWN,UP,LEFT,RIGHT} 
	// BEGIN CODE HERE



	// END CODE HERE 

	// Update speed, position and rotation
	// BEGIN CODE HERE



	// END CODE HERE 

	// Apply transformation
	// BEGIN CODE HERE



	// END CODE HERE

	// Modify rocket fire length
	// BEGIN CODE HERE



	// END CODE HERE

}

std::vector<glm::vec3> Player::getProjectileSpawnPoint() const
{
	std::vector<vec3> results;

	// Fill the vector with the ship's projectile spawn positions
	// BEGIN CODE HERE



	// END CODE HERE

	return results;
}

AABB Player::GetGlobalAABB() const
{
	return core->GetGeneralAABB();
}

std::vector<AABB> Player::GetAABB() const
{
	return core->GetAABBList();
}

bool Player::Intersect(vec3 world_pos) const
{
	return core->Intersect(world_pos);
}
