#pragma once

#include <main.h>

using namespace glm;

struct VertexPositionNormal
{
	vec3 position;
	vec3 normal;
};

struct AABB
{
	vec3 min;
	vec3 max;
};


class Node
{
public:
	static void InitializePreLink(GLuint program);
	static void InitializePostLink(GLuint program);

	Node();
	void SetTransform(const mat4 &transform);
	void AddChild(Node *child);

	AABB GetFullBoundingBox();
	bool Intersect(vec3 world_pos);
	std::vector<AABB> GetAABBList();
	AABB GetGeneralAABB();

protected:

	const vec3 boundaries[8] = {
		vec3(-0.5f,-0.5f,-0.5f),
		vec3(-0.5f,-0.5f,0.5f),
		vec3(-0.5f,0.5f,-0.5f),
		vec3(-0.5f,0.5f,0.5f),
		vec3(0.5f,-0.5f,-0.5f),
		vec3(0.5f,-0.5f,0.5f),
		vec3(0.5f,0.5f,-0.5f),
		vec3(0.5f,0.5f,0.5f)
	};

	glm::mat4 fullTransform() const;

	mat4 _transform;
	std::vector<Node*> _children;
	Node* _parent;

	static GLint uniform_model, uniform_color;
	static GLint attribute_position, attribute_normal;

	void ComputeBoundingBox();
	AABB _boundingBox;
};

class Shape : public Node
{
public:
	virtual void Render();
	virtual ~Shape();

	void color(const vec4& v) { _color = v; }
	const vec4& color() const { return _color; }

	Shape() = default;
	Shape(const Shape& other);


protected:


	GLuint _vertexBuffer, _indexBuffer, _vao;
	vec4 _color;
	std::vector<VertexPositionNormal> _vertices;
	std::vector<uint> _indices;



};

class Box : public Shape
{
public:
	Box(vec4 color);

	virtual void Render() override;
};

class Sphere : public Shape
{
public:
	Sphere(uint iterations, vec4 color);

	virtual void Render() override;
};

class Cylinder : public Shape
{
public:
	Cylinder(uint iterations, vec4 color, double height);
	virtual void Render() override;
private:
	int vertices_count = 0;
};

class Pyramid : public Shape
{
public:
	Pyramid(vec4 color);
	virtual void Render() override;
};