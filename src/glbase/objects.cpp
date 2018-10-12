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

AABB Entity::GetGlobalAABB()
{
	return GetGeneralAABB();
}

std::vector<AABB> Entity::GetAABB()
{
	return GetAABBList();
}

bool Entity::Intersect(vec3 world_pos)
{
	return Node::Intersect(world_pos);
}

Fighter1::Fighter1(const vec3& position, const vec3& velocity, double rate_of_fire, vec3 proj_vel) : Entity(position, velocity, rate_of_fire, proj_vel)
{
	static vec4 forestGreen = vec4(0.133, 0.545, 0.133, 1.000);

	// Primitives
	horizontal = std::make_shared<Box>(forestGreen);
	vertical = std::make_shared<Box>(forestGreen);

	center = std::make_shared<Pyramid>(forestGreen);
	left = std::make_shared<Pyramid>(forestGreen);
	right = std::make_shared<Pyramid>(forestGreen);
	up = std::make_shared<Pyramid>(forestGreen);
	down = std::make_shared<Pyramid>(forestGreen);

	// Hierarchy
	center->AddChild(horizontal.get());
	center->AddChild(vertical.get());

	horizontal->AddChild(left.get());
	horizontal->AddChild(right.get());

	vertical->AddChild(up.get());
	vertical->AddChild(down.get());

	// Transformations
	// XXX make sure I meet all the requirements + Maybe add caps
	horizontal->SetTransform(scale(vec3(2, 1, 1)));
	vertical->SetTransform(scale(vec3(1, 1, 2)));

	center->SetTransform(translate(Position) *
		rotate(mat4(), .5f * pi(), vec3(1, 0, 0)));
	left->SetTransform(scale(vec3(.5f, 1, 1)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), .5f * pi(), vec3(0, 0, 1)));
	right->SetTransform(scale(vec3(.5f, 1, 1)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), -.5f * pi(), vec3(0, 0, 1)));
	up->SetTransform(scale(vec3(1, 1, .5f)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), -.5f * pi(), vec3(1, 0, 0)));
	down->SetTransform(scale(vec3(1, 1, .5f)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), .5f * pi(), vec3(1, 0, 0)));
}

void Fighter1::Render()
{
	horizontal->Render();
	vertical->Render();

	center->Render();
	left->Render();
	right->Render();
	up->Render();
	down->Render();
}

void Fighter1::Update(double dt)
{
	Position += _velocity * decimal(dt);
	animation += dt; // XXX Could cause overflow

	center->SetTransform(translate(Position) *
		rotate(mat4(), .5f * pi(), vec3(1, 0, 0)) *
		rotate(mat4(), animation * animationSpeed, vec3(0, 1, 0)));
}

AABB Fighter1::GetGlobalAABB()
{
	return center->GetGeneralAABB();
}

std::vector<AABB> Fighter1::GetAABB()
{
	return center->GetAABBList();
}

bool Fighter1::Intersect(vec3 world_pos)
{
	return center->Intersect(world_pos);
}

Fighter2::Fighter2(const vec3& position, const vec3& velocity, double rate_of_fire, vec3 proj_vel) : Entity(position, velocity, rate_of_fire, proj_vel)
{

	// Build fighter1 (build a hierarchy using primitives and transformations)
	// TODO CODE HERE



	// END CODE HERE 

}

void Fighter2::Render()
{
	// Render fighter1
	// TODO CODE HERE



	// END CODE HERE 

}

void Fighter2::Update(double dt)
{
	// Update fighter1's position
	// TODO CODE HERE



	// END CODE HERE 

}

AABB Fighter2::GetGlobalAABB()
{
	return {}; // TODO
}

std::vector<AABB> Fighter2::GetAABB()
{
	return {}; // TODO
}

bool Fighter2::Intersect(vec3 world_pos)
{
	return false; // TODO
}
