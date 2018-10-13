#pragma once

#include "scene.h"
#include <list>

class Projectile : public Node
{
public:
	Projectile() = delete;
	Projectile(const vec3 &position, const vec3 &velocity, const vec4 &color_i, const vec4 &color_o, bool friendly);

	virtual void Render();
	virtual void Update(double dt);

	void velocity(const vec3& v) { _velocity = v; }
	const vec3& velocity() const { return _velocity; }

	void position(const vec3& p) { _position = p; }
	const vec3& position() const { return _position; }

	bool friendly() const { return _friendly; }
protected:
	Sphere _inner, _outer;

	vec3 _position;
	vec3 _velocity;
	bool _friendly;
};

class Entity : public Node
{
public:
	Entity(const vec3 &position, const vec3 &velocity, double rate_of_fire, vec3 proj_vel = vec3(0.0f, 0.0f, 5.0f))
	{
		Position = position;
		_velocity = velocity;
		rof = rate_of_fire;
		projectile_vel = proj_vel;
	};

	virtual void Render() = 0;
	virtual void Update(double dt) = 0;

	std::vector<vec3> GetProjectileSpawnPoint();
	vec3 Position;
	double last_shot = 0.0f;
	double rof = 2.0f;
	vec3 projectile_vel = vec3(0.0f, 0.0f, 5.0f);
	uint score;

	virtual AABB GetGlobalAABB();
	virtual std::vector<AABB> GetAABB();
	virtual bool Intersect(vec3 world_pos);

protected:

	vec3 _velocity;

};

class Fighter1 : public Entity
{
public:
	Fighter1() = delete;
	Fighter1(const vec3 &position, const vec3 &velocity, double rate_of_fire, vec3 proj_vel = vec3(0.0f, 0.0f, 5.0f));

	virtual void Render() override;
	virtual void Update(double dt) override;

	virtual AABB GetGlobalAABB();
	virtual std::vector<AABB> GetAABB();
	virtual bool Intersect(vec3 world_pos);

private:
	// Meshes
	std::shared_ptr<Box> horizontal;
	std::shared_ptr<Box> vertical;

	std::shared_ptr<Pyramid> center;
	std::shared_ptr<Pyramid> left;
	std::shared_ptr<Pyramid> right;
	std::shared_ptr<Pyramid> up;
	std::shared_ptr<Pyramid> down;

	float animation = 0;
	float animSpeed = 2;
};

class Fighter2 : public Entity
{
public:
	Fighter2() = delete;
	Fighter2(const vec3 &position, const vec3 &velocity, double rate_of_fire, vec3 proj_vel = vec3(0.0f, 0.0f, 5.0f));

	virtual void Render() override;
	virtual void Update(double dt) override;

	virtual AABB GetGlobalAABB();
	virtual std::vector<AABB> GetAABB();
	virtual bool Intersect(vec3 world_pos);

private:
	// Meshes
	std::shared_ptr<Box> center;
	std::shared_ptr<Pyramid> top;

	std::shared_ptr<Cylinder> left;
	std::shared_ptr<Cylinder> right;

	std::shared_ptr<Sphere> backLeft;
	std::shared_ptr<Sphere> backRight;

	bool scaleUp = true;
};
