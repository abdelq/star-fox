#include "player.h"
#include <glm/ext.hpp>

#define X_AXIS vec3(1, 0, 0)
Player::Player()
{
	std::srand(std::time(0));

	static vec4 oxBlood   = vec4(0.502, 0.000, 0.125, 1.000);
	static vec4 celeste   = vec4(0.698, 1.000, 1.000, 1.000);
	static vec4 silver    = vec4(0.753, 0.753, 0.753, 1.000);
	static vec4 oldSilver = vec4(0.518, 0.518, 0.510, 1.000);
	static vec4 flame     = vec4(0.886, 0.345, 0.133, 1.000);

	// Primitives
	core = std::make_shared<Pyramid>(oxBlood);

	back_core = std::make_shared<Cylinder>(16, celeste, 1);
	left_aileron = std::make_shared<Box>(silver);
	right_aileron = std::make_shared<Box>(silver);

	left_wing = std::make_shared<Box>(silver);
	left_rocket = std::make_shared<Cylinder>(8, oldSilver, .5);
	left_rocket_fire = std::make_shared<Cylinder>(8, flame, .25);
	left_2_rocket = std::make_shared<Cylinder>(8, oldSilver, .5);
	left_2_rocket_fire = std::make_shared<Cylinder>(8, flame, .25);

	right_wing = std::make_shared<Box>(silver);
	right_rocket = std::make_shared<Cylinder>(8, oldSilver, .5);
	right_rocket_fire = std::make_shared<Cylinder>(8, flame, .25);
	right_2_rocket = std::make_shared<Cylinder>(8, oldSilver, .5);
	right_2_rocket_fire = std::make_shared<Cylinder>(8, flame, .25);

	// Hierarchy
	core->AddChild(back_core.get());
	core->AddChild(left_wing.get());
	core->AddChild(right_wing.get());

	back_core->AddChild(left_aileron.get());
	back_core->AddChild(right_aileron.get());

	left_wing->AddChild(left_rocket.get());
	left_wing->AddChild(left_2_rocket.get());
	left_rocket->AddChild(left_rocket_fire.get());
	left_2_rocket->AddChild(left_2_rocket_fire.get());

	right_wing->AddChild(right_rocket.get());
	right_wing->AddChild(right_2_rocket.get());
	right_rocket->AddChild(right_rocket_fire.get());
	right_2_rocket->AddChild(right_2_rocket_fire.get());

	// Transformations
	// XXX These and others are in header to add and to use properly
	// XXX Follow proper hierarchy from file regarding transfos
	core_trans = scale(vec3(1, 1, .25)) * rotate(mat4(), -.5f * pi(), X_AXIS);
	inv_core_trans = inverse(core_trans);

	core->SetTransform(core_trans);

	back_core->SetTransform(
		translate(vec3(0, -.5, 0)) *
		scale(vec3(1.25, .5, 1.25))
	);

	left_aileron->SetTransform(
		scale(vec3(1, .5, .25)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		translate(vec3(-.75, 1, -1.5)) *
		shearY3D(mat4(), -1.f, .25f)
	);

	right_aileron->SetTransform(
		scale(vec3(1, .5, .25)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		translate(vec3(.75, 1, -1.5)) *
		shearY3D(mat4(), 1.f, .25f)
	);

	left_wing->SetTransform(
		scale(vec3(1.25, .25, .25)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		translate(vec3(-.75, -2, 1.75)) *
		shearZ3D(shearY3D(mat4(), 1.f, .25f), -1.f, .25f)
	);

	right_wing->SetTransform(
		scale(vec3(1.25, .25, .25)) *
		rotate(mat4(), .5f * pi(), X_AXIS) *
		translate(vec3(.75, -2, 1.75)) *
		shearZ3D(shearY3D(mat4(), -1.f, .25f), 1.f, .25f)
	);

	left_rocket->SetTransform(
		inverse(shearZ3D(shearY3D(mat4(), 1.f, .25f), -1.f, .25f)) * // XXX
		rotate(mat4(), -.5f * pi(), X_AXIS) * // XXX
		scale(vec3(.25, 2, 1)) *
		translate(vec3(1 , 0, -.75))
	);

	left_2_rocket->SetTransform(
		inverse(shearZ3D(shearY3D(mat4(), 1.f, .25f), -1.f, .25f)) * // XXX
		rotate(mat4(), -.5f * pi(), X_AXIS) * // XXX
		scale(vec3(.25, 2, 1)) *
		translate(vec3(-1 , 0, -1.25))
	);

	right_rocket->SetTransform(
		inverse(shearZ3D(shearY3D(mat4(), -1.f, .25f), 1.f, .25f)) * // XXX
		rotate(mat4(), -.5f * pi(), X_AXIS) * // XXX
		scale(vec3(.25, 2, 1)) *
		translate(vec3(-1 , 0, -.75))
	);

	right_2_rocket->SetTransform(
		inverse(shearZ3D(shearY3D(mat4(), -1.f, .25f), 1.f, .25f)) * // XXX
		rotate(mat4(), -.5f * pi(), X_AXIS) * // XXX
		scale(vec3(.25, 2, 1)) *
		translate(vec3(1 , 0, -1.25))
	);

	// XXX A lot of repetition
	left_rocket_fire->SetTransform(
		scale(vec3(.75, 1, .75)) *
		translate(vec3(0, -.25, 0))
	);

	left_2_rocket_fire->SetTransform(
		scale(vec3(.75, 1, .75)) *
		translate(vec3(0, -.25, 0))
	);

	right_rocket_fire->SetTransform(
		scale(vec3(.75, 1, .75)) *
		translate(vec3(0, -.25, 0))
	);

	right_2_rocket_fire->SetTransform(
		scale(vec3(.75, 1, .75)) *
		translate(vec3(0, -.25, 0))
	);
}

void Player::Render()
{
	core->Render();

	back_core->Render();
	left_aileron->Render();
	right_aileron->Render();

	left_wing->Render();
	left_rocket->Render();
	left_rocket_fire->Render();
	left_2_rocket->Render();
	left_2_rocket_fire->Render();

	right_wing->Render();
	right_rocket->Render();
	right_rocket_fire->Render();
	right_2_rocket->Render();
	right_2_rocket_fire->Render();
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
