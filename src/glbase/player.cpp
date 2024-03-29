#include "player.h"
#include <glm/ext.hpp>

#define X_AXIS vec3(1, 0, 0)
#define Y_AXIS vec3(0, 1, 0)
#define Z_AXIS vec3(0, 0, 1)
Player::Player()
{
	srand(time(0));
	memset(input, 0, sizeof input);

	static vec4 oxBlood = vec4(128, 0, 32, 255) / 255;
	static vec4 celeste = vec4(178, 255, 255, 255) / 255;
	static vec4 silver = vec4(192, 192, 192, 255) / 255;
	static vec4 oldSilver = vec4(132, 132, 130, 255) / 255;
	static vec4 flame = vec4(226, 88, 34, 255) / 255;

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

	mat4 left_wing_trans = rotate(mat4(), .5f * pi(), X_AXIS) *
		shearZ3D(shearY3D(mat4(), 1.f, .25f), -1.f, .25f);
	left_wing->SetTransform(
		scale(vec3(1.25, .25, .25)) *
		left_wing_trans *
		translate(vec3(-.75, -2, 1.75))
	);

	mat4 right_wing_trans = rotate(mat4(), .5f * pi(), X_AXIS) *
		shearZ3D(shearY3D(mat4(), -1.f, .25f), 1.f, .25f);
	right_wing->SetTransform(
		scale(vec3(1.25, .25, .25)) *
		right_wing_trans *
		translate(vec3(.75, -2, 1.75))
	);

	left_rocket->SetTransform(
		inverse(left_wing_trans) *
		scale(vec3(.25, 2, 1)) *
		translate(vec3(1 , 0, -.75))
	);

	left_2_rocket->SetTransform(
		inverse(left_wing_trans) *
		scale(vec3(.25, 2, 1)) *
		translate(vec3(-1 , 0, -1.25))
	);

	right_rocket->SetTransform(
		inverse(right_wing_trans) *
		scale(vec3(.25, 2, 1)) *
		translate(vec3(-1 , 0, -.75))
	);

	right_2_rocket->SetTransform(
		inverse(right_wing_trans) *
		scale(vec3(.25, 2, 1)) *
		translate(vec3(1 , 0, -1.25))
	);

	left_rocket_trans = left_2_rocket_trans =
		right_rocket_trans = right_2_rocket_trans =
		scale(vec3(.5, 2, .5)) * translate(vec3(0, -.25, 0));

	left_rocket_fire->SetTransform(left_rocket_trans);
	left_2_rocket_fire->SetTransform(left_2_rocket_trans);
	right_rocket_fire->SetTransform(right_rocket_trans);
	right_2_rocket_fire->SetTransform(right_2_rocket_trans);
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
	decimal delta = decimal(dt);

	// Acceleration
	accel = vec2(input[RIGHT] - input[LEFT], input[UP] - input[DOWN]);
	if (accel != vec2(0))
		accel = normalize(accel);
	accel *= accel_value;

	// Speed
	speed += accel * delta;

	vec2 speed_sign = sign(speed);
	speed -= speed_sign * trainee * delta;
	if (sign(speed.x) != speed_sign.x)
		speed.x = 0;
	if (sign(speed.y) != speed_sign.y)
		speed.y = 0;

	speed = clamp(speed, -max_speed, max_speed);

	// Position
	Position.x = clamp(Position.x + speed.x * delta, -8.f, 8.f);
	Position.y = clamp(Position.y + speed.y * delta, -7.f, 7.f);

	// Rotation
	rotation += accel * delta;

	vec2 rotation_sign = sign(rotation);
	rotation -= rotation_sign * trainee * delta;
	if (sign(rotation[0]) != rotation_sign[0])
		rotation[0] = 0;
	if (sign(rotation[1]) != rotation_sign[1])
		rotation[1] = 0;

	rotation[0] = clamp(rotation[0], -max_x_rotation, max_x_rotation);
	rotation[1] = clamp(rotation[1], -max_z_rotation, max_z_rotation);

	// Applying transformations
	core->SetTransform(
		translate(Position) *
		core_trans *
		rotate(mat4(),  rotation[1]/180 * pi() * rotation_coeff, X_AXIS) *
		rotate(mat4(), -rotation[0]/180 * pi() * rotation_coeff, Z_AXIS)
	);

	// Modifying rocket fire length
	decimal fireLen = .5 * l2Norm(vec3(speed, 0))/l2Norm(vec3(max_speed, max_speed, 0)) - .5;

	left_rocket_fire->SetTransform(
		left_rocket_trans *
		translate(vec3(0, -fireLen * rand()/RAND_MAX, 0))
	);
	left_2_rocket_fire->SetTransform(
		left_2_rocket_trans *
		translate(vec3(0, -fireLen * rand()/RAND_MAX, 0))
	);

	right_rocket_fire->SetTransform(
		right_rocket_trans *
		translate(vec3(0, -fireLen * rand()/RAND_MAX, 0))
	);
	right_2_rocket_fire->SetTransform(
		right_2_rocket_trans *
		translate(vec3(0, -fireLen * rand()/RAND_MAX, 0))
	);
}

std::vector<glm::vec3> Player::getProjectileSpawnPoint() const
{
	return {
		Position + vec3(-1.0, +0.0, -0.5),
		Position + vec3(+1.0, -0.0, -0.5),
		Position + vec3(-0.0, +0.5, -0.5),
		Position + vec3(+0.0, -0.5, -0.5),
	};
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
