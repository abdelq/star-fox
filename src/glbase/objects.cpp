#include "objects.h"
#include <glm/gtx/transform2.hpp>

Projectile::Projectile(const vec3 &position, const vec3 &velocity, const vec4 &color_i, const vec4 &color_o, bool friendly) :
	_inner(1, color_i),
	_outer(1, color_o),
	_position(position),
	_velocity(velocity),
	_friendly(friendly)
{
	AddChild(&_inner);
	_inner.AddChild(&_outer);

	_outer.SetTransform(glm::scale(vec3(2)));
}

void Projectile::Render()
{
	_inner.Render();
	_outer.Render();
}

void Projectile::Update(double dt)
{
	_position += _velocity * decimal(dt);

	SetTransform(glm::translate(_position) * scale(vec3(0.1f)));
}

std::vector<vec3> Entity::GetProjectileSpawnPoint()
{
	return std::vector<vec3>{Position + vec3(0.0f, 0.0f, 0.5f)};
}

Fighter1::Fighter1(const vec3& position, const vec3& velocity, double rate_of_fire, vec3 proj_vel) : Entity(position, velocity, rate_of_fire, proj_vel)
{

	// Build fighter1 (build a hierarchy using primitives and transformations)
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Fighter1::Render()
{
	// Render fighter1
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Fighter1::Update(double dt)
{
	// Update fighter1's position
	// BEGIN CODE HERE



	// END CODE HERE 

}

Fighter2::Fighter2(const vec3& position, const vec3& velocity, double rate_of_fire, vec3 proj_vel) : Entity(position, velocity, rate_of_fire, proj_vel)
{

	// Build fighter1 (build a hierarchy using primitives and transformations)
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Fighter2::Render()
{
	// Render fighter1
	// BEGIN CODE HERE



	// END CODE HERE 

}

void Fighter2::Update(double dt)
{
	// Update fighter1's position
	// BEGIN CODE HERE



	// END CODE HERE 

}
