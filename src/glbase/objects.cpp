#include "objects.h"
#include <glm/gtx/transform2.hpp>

#define X_AXIS vec3(1, 0, 0)
#define Y_AXIS vec3(0, 1, 0)
#define Z_AXIS vec3(0, 0, 1)
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
	score = 1000;

	static vec4 chartreuse = vec4(223, 255, 0, 255) / 255;
	static vec4 forestGreen = vec4(34, 139, 34, 255) / 255;

	// Primitives
	horizontal = std::make_shared<Box>(forestGreen);
	vertical = std::make_shared<Box>(forestGreen);

	center = std::make_shared<Pyramid>(chartreuse);
	left = std::make_shared<Pyramid>(chartreuse);
	right = std::make_shared<Pyramid>(chartreuse);
	up = std::make_shared<Pyramid>(chartreuse);
	down = std::make_shared<Pyramid>(chartreuse);

	// Hierarchy
	center->AddChild(horizontal.get());
	center->AddChild(vertical.get());

	horizontal->AddChild(left.get());
	horizontal->AddChild(right.get());

	vertical->AddChild(up.get());
	vertical->AddChild(down.get());

	// Transformations
	horizontal->SetTransform(scale(vec3(2, 1, 1)));
	vertical->SetTransform(scale(vec3(1, 1, 2)));

	center->SetTransform(translate(Position) *
		rotate(mat4(), .5f * pi(), X_AXIS));
	left->SetTransform(scale(vec3(.5, 1, 1)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), .5f * pi(), Z_AXIS));
	right->SetTransform(scale(vec3(.5, 1, 1)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), -.5f * pi(), Z_AXIS));
	up->SetTransform(scale(vec3(1, 1, .5)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), -.5f * pi(), X_AXIS));
	down->SetTransform(scale(vec3(1, 1, .5)) * translate(vec3(0, 1, 0)) *
		rotate(mat4(), .5f * pi(), X_AXIS));
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
	animation += dt;

	center->SetTransform(translate(Position) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		rotate(mat4(), animation * animSpeed, Y_AXIS));
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
	score = 2000;

	static vec4 chartreuse = vec4(223, 255, 0, 255) / 255;
	static vec4 forestGreen = vec4(34, 139, 34, 255) / 255;

	// Primitives
	center = std::make_shared<Box>(forestGreen);
	top = std::make_shared<Pyramid>(forestGreen);

	left = std::make_shared<Cylinder>(16, chartreuse, 1);
	right = std::make_shared<Cylinder>(16, chartreuse, 1);

	backLeft = std::make_shared<Sphere>(1, forestGreen);
	backRight = std::make_shared<Sphere>(1, forestGreen);

	// Hierarchy
	center->AddChild(top.get());

	center->AddChild(left.get());
	center->AddChild(right.get());

	left->AddChild(backLeft.get());
	right->AddChild(backRight.get());

	// Transformations
	center->SetTransform(scale(vec3(1, .25, 1)));

	top->SetTransform(scale(vec3(1, 2, 1)) * translate(vec3(0, .25, 0)));

	left->SetTransform(
		scale(vec3(.5, 2, 2)) *
		translate(vec3(-1, 0, -.5)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		shearY3D(rotate(mat4(), -.5f * pi(), Y_AXIS), .5f, .25f)
	);
	right->SetTransform(
		scale(vec3(.5, 2, 2)) *
		translate(vec3(1, 0, -.5)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		shearY3D(rotate(mat4(), -.5f * pi(), Y_AXIS), .5f, .25f)
	);

	backLeft->SetTransform(
		inverse(shearY3D(rotate(mat4(), -.5f * pi(), Y_AXIS), .5f, .25f)) *
		scale(vec3(.5)) *
		translate(vec3(0, -.75, 0))
	);
	backRight->SetTransform(
		inverse(shearY3D(rotate(mat4(), -.5f * pi(), Y_AXIS), .5f, .25f)) *
		scale(vec3(.5)) *
		translate(vec3(0, -.75, 0))
	);
}

void Fighter2::Render()
{
	center->Render();
	top->Render();

	left->Render();
	right->Render();

	backLeft->Render();
	backRight->Render();
}

void Fighter2::Update(double dt)
{
	Position += _velocity * decimal(dt);
	scaleUp = !scaleUp;

	center->SetTransform(
		translate(Position) *
		scale(vec3(1, .25, 1)) *
		scale(scaleUp ? vec3(2) : vec3(.5))
	);
}

AABB Fighter2::GetGlobalAABB()
{
	return center->GetGeneralAABB();
}

std::vector<AABB> Fighter2::GetAABB()
{
	return center->GetAABBList();
}

bool Fighter2::Intersect(vec3 world_pos)
{
	return center->Intersect(world_pos);
}
