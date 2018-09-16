#pragma once
#include "scene.h"
#include "objects.h"
#include <random>

class Player
{
public:
	enum Input {
		UP,
		DOWN,
		LEFT,
		RIGHT,
		SPACE,
		ENUM_END
	};

	Player();
	void Render();
	void Update(double dt);
	std::vector<vec3> getProjectileSpawnPoint() const;
	uint lifes = 5;
	int score = 0;

	bool input[ENUM_END];

	//Projectile related
	glm::vec3 projectile_speed = vec3(0.0f, 0.0f, -50.0f);
	glm::vec4 projectile_color_out = glm::vec4(0.7f, 0.0f, 0.0f, 0.8f);
	glm::vec4 projectile_color_in = glm::vec4(0.7f, 0.0f, 0.0f, 1.0f);
	double shot_delay = 0.25; //s
	double last_shot = 0.0;
	vec3 Position = vec3(0.0f);

	AABB GetGlobalAABB() const;
	std::vector<AABB> GetAABB() const;
	bool Intersect(vec3 world_pos) const;
	bool god_mode = false;

private:



	//Meshes
	std::shared_ptr<Pyramid> core;
	std::shared_ptr<Box> right_wing;
	std::shared_ptr<Box> left_wing;
	std::shared_ptr<Cylinder> left_rocket;
	std::shared_ptr<Cylinder> right_rocket;
	std::shared_ptr<Cylinder> left_rocket_fire;
	std::shared_ptr<Cylinder> right_rocket_fire;
	std::shared_ptr<Cylinder> left_2_rocket;
	std::shared_ptr<Cylinder> right_2_rocket;
	std::shared_ptr<Cylinder> left_2_rocket_fire;
	std::shared_ptr<Cylinder> right_2_rocket_fire;
	std::shared_ptr<Cylinder> back_core;
	std::shared_ptr<Box> right_aileron;
	std::shared_ptr<Box> left_aileron;

	//Transformation related
	mat4 core_trans;
	mat4 inv_core_trans;

	mat4 right_2_rocket_trans;
	mat4 left_2_rocket_trans;
	mat4 right_rocket_trans;
	mat4 left_rocket_trans;

	//Movement related

	float recovery_time = 0.1f;
	float trainee = 10.0f;
	vec2 accel = vec2(0.0f);
	float accel_value = 30.0f;
	vec2 speed = vec2(0.0f);//xy
	float max_speed = 20.0f;
	float max_x_rotation = 15.0f;
	float max_z_rotation = 35.0f;
	float rotation_coeff = 2.5f;
	float size = 2.0f;

};