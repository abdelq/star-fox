#include "scene.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#pragma region NODE

GLint Node::uniform_model = -1, Node::uniform_color = -1;
GLint Node::attribute_position = 1, Node::attribute_normal = 2;

void Node::InitializePreLink(GLuint program)
{
	glBindAttribLocation(program, attribute_position, "in_position");
	glBindAttribLocation(program, attribute_normal, "in_normal");
}

void Node::InitializePostLink(GLuint program)
{
	uniform_model = glGetUniformLocation(program, "model");
	uniform_color = glGetUniformLocation(program, "color");
}

Node::Node()
	: _transform(), _children(), _parent(nullptr)
{ }

void Node::SetTransform(const mat4 &transform)
{
	_transform = transform;
}

void Node::AddChild(Node *child)
{
	_children.push_back(child);
	child->_parent = this;
}

glm::mat4 Node::fullTransform() const
{
	if (_parent == nullptr)
		return _transform;
	else
		return _parent->fullTransform() * _transform;

}

AABB Node::GetFullBoundingBox()
{
	ComputeBoundingBox();
	return _boundingBox;
}

bool Node::Intersect(vec3 world_pos)
{
	bool intersect = true;

	AABB gen = GetGeneralAABB();
	for (int i = 0; i < 3; ++i)
		intersect &= gen.min[i] < world_pos[i] && gen.max[i] > world_pos[i];

	if (intersect)
	{
		for (AABB box : GetAABBList())
		{
			if (box.min.x < world_pos.x && box.max.x > world_pos.x &&
				box.min.y < world_pos.y && box.max.y > world_pos.y &&
				box.min.z < world_pos.z && box.max.z > world_pos.z)
			{
				return true;
			}
		}
	}

	return false;
}

std::vector<AABB> Node::GetAABBList()
{
	std::vector<AABB> results;
	results.push_back(GetFullBoundingBox());
	for (uint i = 0; i < _children.size(); ++i)
	{
		std::vector<AABB> tree = _children[i]->GetAABBList();
		for (uint j = 0; j < tree.size(); ++j)
		{
			results.push_back(tree[j]);
		}
	}
	return results;
}

AABB Node::GetGeneralAABB()
{
	auto full_bounding_box = GetFullBoundingBox();
	vec3 min = full_bounding_box.min;
	vec3 max = full_bounding_box.max;

	for (uint i = 0; i < _children.size(); ++i)
	{
		auto general_aabb = _children[i]->GetGeneralAABB();
		vec3 child_min = general_aabb.min;
		vec3 child_max = general_aabb.max;

		min.x = glm::min(min.x, child_min.x);
		min.x = glm::min(min.x, child_max.x);
		min.y = glm::min(min.y, child_min.y);
		min.y = glm::min(min.y, child_max.y);
		min.z = glm::min(min.z, child_min.z);
		min.z = glm::min(min.z, child_max.z);

		max.x = glm::max(max.x, child_min.x);
		max.x = glm::max(max.x, child_max.x);
		max.y = glm::max(max.y, child_min.y);
		max.y = glm::max(max.y, child_max.y);
		max.z = glm::max(max.z, child_min.z);
		max.z = glm::max(max.z, child_max.z);
	}

	return{ min, max };
}

void Node::ComputeBoundingBox()
{
	mat4 fullTrans = fullTransform();

	_boundingBox = { vec3(100), vec3(-100) };
	for (vec3 b : boundaries)
	{
		vec4 bTrans = fullTrans * vec4(b, 1);
		for (int i = 0; i < 3; ++i)
		{
			_boundingBox.min[i] = min(bTrans[i], _boundingBox.min[i]);
			_boundingBox.max[i] = max(bTrans[i], _boundingBox.max[i]);
		}
	}
}


#pragma endregion

#pragma region SHAPE

void Shape::Render()
{
	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(fullTransform()));
	glUniform4fv(uniform_color, 1, glm::value_ptr(_color));

	if (_color.a < 1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
	}
	else
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

Shape::~Shape()
{
	if (_vertexBuffer != BAD_BUFFER)
		glDeleteBuffers(1, &_vertexBuffer);

	if (_indexBuffer != BAD_BUFFER)
		glDeleteBuffers(1, &_indexBuffer);

	glDeleteVertexArrays(1, &_vao);
}

Shape::Shape(const Shape& other)
{
	_vertices = other._vertices;
	_indices = other._indices;
	_color = other._color;
	_transform = other._transform;

	// Create Vertex Array Object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate Vertex Buffer
	glGenBuffers(1, &_vertexBuffer);
	if (!_indices.empty())
		glGenBuffers(1, &_indexBuffer);

	// Fill Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexPositionNormal), _vertices.data(), GL_STATIC_DRAW);
	if (!_indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), _indices.data(), GL_STATIC_DRAW);
	}

	// Set Vertex Attributes
	glEnableVertexAttribArray(attribute_position);
	glVertexAttribPointer(attribute_position, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)0);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(attribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)(0 + sizeof(vec3)));

	glBindVertexArray(0);
}

#pragma endregion

#pragma region BOX

Box::Box(vec4 color)
{
	_vertexBuffer = _indexBuffer = BAD_BUFFER;

	_color = color;

	VertexPositionNormal vertices[36] = {
		{ vec3(0, 0, 0), vec3(0, -1, 0) },
		{ vec3(1, 0, 0), vec3(0, -1, 0) },
		{ vec3(0, 0, 1), vec3(0, -1, 0) },

		{ vec3(0, 0, 1), vec3(0, -1, 0) },
		{ vec3(1, 0, 0), vec3(0, -1, 0) },
		{ vec3(1, 0, 1), vec3(0, -1, 0) },


		{ vec3(1, 0, 0), vec3(1, 0, 0) },
		{ vec3(1, 1, 0), vec3(1, 0, 0) },
		{ vec3(1, 0, 1), vec3(1, 0, 0) },

		{ vec3(1, 0, 1), vec3(1, 0, 0) },
		{ vec3(1, 1, 0), vec3(1, 0, 0) },
		{ vec3(1, 1, 1), vec3(1, 0, 0) },


		{ vec3(1, 1, 0), vec3(0, 1, 0) },
		{ vec3(0, 1, 1), vec3(0, 1, 0) },
		{ vec3(1, 1, 1), vec3(0, 1, 0) },

		{ vec3(0, 1, 0), vec3(0, 1, 0) },
		{ vec3(0, 1, 1), vec3(0, 1, 0) },
		{ vec3(1, 1, 0), vec3(0, 1, 0) },


		{ vec3(0, 1, 1), vec3(-1, 0, 0) },
		{ vec3(0, 1, 0), vec3(-1, 0, 0) },
		{ vec3(0, 0, 1), vec3(-1, 0, 0) },

		{ vec3(0, 1, 0), vec3(-1, 0, 0) },
		{ vec3(0, 0, 0), vec3(-1, 0, 0) },
		{ vec3(0, 0, 1), vec3(-1, 0, 0) },


		{ vec3(0, 0, 1), vec3(0, 0, 1) },
		{ vec3(1, 0, 1), vec3(0, 0, 1) },
		{ vec3(0, 1, 1), vec3(0, 0, 1) },

		{ vec3(1, 0, 1), vec3(0, 0, 1) },
		{ vec3(1, 1, 1), vec3(0, 0, 1) },
		{ vec3(0, 1, 1), vec3(0, 0, 1) },


		{ vec3(0, 0, 0), vec3(0, 0, -1) },
		{ vec3(0, 1, 0), vec3(0, 0, -1) },
		{ vec3(1, 0, 0), vec3(0, 0, -1) },

		{ vec3(1, 0, 0), vec3(0, 0, -1) },
		{ vec3(0, 1, 0), vec3(0, 0, -1) },
		{ vec3(1, 1, 0), vec3(0, 0, -1) }
	};
	_vertices.assign(&vertices[0], &vertices[36]);

	for (uint x = 0; x < 36; x++)
		_vertices[x].position -= 0.5;

	// Create Vertex Array Object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate Vertex Buffer
	glGenBuffers(1, &_vertexBuffer);

	// Fill Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexPositionNormal), _vertices.data(), GL_STATIC_DRAW);

	// Set Vertex Attributes
	glEnableVertexAttribArray(attribute_position);
	glVertexAttribPointer(attribute_position, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)0);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(attribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)(0 + sizeof(vec3)));

	glBindVertexArray(0);

	debugGLError();
}

void Box::Render()
{
	Shape::Render();

	glBindVertexArray(_vao);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

#pragma endregion

#pragma region SPHERE

// http://codingincircles.com/2009/09/geosphere-generation/

Sphere::Sphere(uint iterations, vec4 color)
{
	_vertexBuffer = _indexBuffer = BAD_BUFFER;

	_color = color;

	std::vector<vec3> vertices;
	std::vector<ivec3> indices;

	decimal insideExtent = cos(pi() / 5);
	decimal sideLength = 2 * sin(pi() / 5);
	decimal Cx = cos(pi() / 10);
	decimal Cz = sin(pi() / 10);
	decimal H1 = sqrt((sideLength)* (sideLength)-1);
	decimal H2 = sqrt((insideExtent + 1) * (insideExtent + 1) - insideExtent * insideExtent);
	decimal Y2 = 0.5f * (H2 - H1);
	decimal Y1 = Y2 + H1;
	decimal r = 1;
	decimal s = sideLength;
	decimal h = insideExtent;

	//create the icosahedron
	vertices.push_back(vec3(0, Y1, 0));	//a
	vertices.push_back(vec3(0, Y2, r));	//b
	vertices.push_back(vec3(Cx, Y2, Cz));	//c
	vertices.push_back(vec3(0.5f * s, Y2, -h));	//d
	vertices.push_back(vec3(-0.5f * s, Y2, -h));	//e
	vertices.push_back(vec3(-Cx, Y2, Cz));	//f
	vertices.push_back(vec3(0, -Y2, -r));	//g
	vertices.push_back(vec3(-Cx, -Y2, -Cz));	//h
	vertices.push_back(vec3(-0.5f * s, -Y2, h));	//i
	vertices.push_back(vec3(0.5f * s, -Y2, h));	//j
	vertices.push_back(vec3(Cx, -Y2, -Cz));	//k
	vertices.push_back(vec3(0, -Y1, 0));	//l

	//create the indices list
	indices.push_back(ivec3(0, 1, 2));
	indices.push_back(ivec3(0, 2, 3));
	indices.push_back(ivec3(0, 3, 4));
	indices.push_back(ivec3(0, 4, 5));
	indices.push_back(ivec3(0, 5, 1));
	indices.push_back(ivec3(1, 8, 9));
	indices.push_back(ivec3(9, 2, 1));
	indices.push_back(ivec3(2, 9, 10));
	indices.push_back(ivec3(10, 3, 2));
	indices.push_back(ivec3(3, 10, 6));
	indices.push_back(ivec3(6, 4, 3));
	indices.push_back(ivec3(4, 6, 7));
	indices.push_back(ivec3(7, 5, 4));
	indices.push_back(ivec3(5, 7, 8));
	indices.push_back(ivec3(8, 1, 5));
	indices.push_back(ivec3(11, 6, 10));
	indices.push_back(ivec3(11, 10, 9));
	indices.push_back(ivec3(11, 9, 8));
	indices.push_back(ivec3(11, 8, 7));
	indices.push_back(ivec3(11, 7, 6));

	for (uint i = 0; i < iterations; ++i)
	{
		std::vector<ivec3> indices2(indices.size());
		for (ivec3 indexTriangle : indices)
		{
			vec3 newVectorOne = mix(vertices[indexTriangle.x], vertices[indexTriangle.y], 0.5f);
			vec3 newVectorTwo = mix(vertices[indexTriangle.y], vertices[indexTriangle.z], 0.5f);
			vec3 newVectorThree = mix(vertices[indexTriangle.z], vertices[indexTriangle.x], 0.5f);
			vertices.push_back(newVectorOne);
			vertices.push_back(newVectorTwo);
			vertices.push_back(newVectorThree);

			indices2.push_back(ivec3(indexTriangle.x, vertices.size() - 3, vertices.size() - 1));
			indices2.push_back(ivec3(vertices.size() - 3, indexTriangle.y, vertices.size() - 2));
			indices2.push_back(ivec3(vertices.size() - 2, indexTriangle.z, vertices.size() - 1));
			indices2.push_back(ivec3(vertices.size() - 3, vertices.size() - 2, vertices.size() - 1));
		}

		std::swap(indices, indices2);
	}

	_vertices.reserve(vertices.size());
	_indices.reserve(indices.size());

	for (vec3& v : vertices)
		_vertices.push_back({ normalize(v), normalize(v) });

	for (ivec3& v : indices)
	{
		_indices.push_back(v.x);
		_indices.push_back(v.y);
		_indices.push_back(v.z);
	}

	// Create Vertex Array Object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate Vertex Buffer
	glGenBuffers(1, &_vertexBuffer);
	glGenBuffers(1, &_indexBuffer);

	// Fill Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexPositionNormal), _vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), _indices.data(), GL_STATIC_DRAW);

	// Set Vertex Attributes
	glEnableVertexAttribArray(attribute_position);
	glVertexAttribPointer(attribute_position, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)0);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(attribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)(0 + sizeof(vec3)));

	glBindVertexArray(0);

	debugGLError();
}

void Sphere::Render()
{
	Shape::Render();

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

#pragma endregion

#pragma region CYLINDER

// https://gamedev.net/forums/topic/359467-draw-cylinder-with-triangle-strips

Cylinder::Cylinder(uint iterations, vec4 color, double height)
{
	_vertexBuffer = _indexBuffer = BAD_BUFFER;

	_color = color;

	std::vector<vec3> vertices;
	std::vector<ivec3> indices;

	decimal iter = 2*pi() / iterations;
	decimal h = height * .5;

	// Create the vertices list
	vertices.reserve(iterations * 2 + 2);
	for (uint i = 0; i < iterations; ++i)
		vertices.push_back(vec3(.5 * cos(i * iter), h, .5 * sin(i * iter)));
	for (uint i = 0; i < iterations; ++i)
		vertices.push_back(vec3(.5 * cos(i * iter), -h, .5 * sin(i * iter)));
	vertices.push_back(vec3(0, h, 0));
	vertices.push_back(vec3(0, -h, 0));

	// Create the indices list
	indices.reserve(iterations * 4);
	for (uint i = 0; i < iterations; ++i) // Sides
	{
		indices.push_back(ivec3(i, i+iterations, (i+1)%iterations));
		indices.push_back(ivec3((i+1)%iterations + iterations, (i+1)%iterations, i+iterations));
	}
	for (uint i = 0; i < iterations; ++i) // Caps
	{
		indices.push_back(ivec3(iterations * 2, i, (i+1)%iterations));
		indices.push_back(ivec3(iterations * 2 + 1, (i+1)%iterations + iterations, i+iterations));
	}

	_vertices.reserve(vertices.size());
	_indices.reserve(indices.size());

	for (vec3& v : vertices)
		_vertices.push_back({ v + vec3(0, h, 0), normalize(v) });

	for (ivec3& v : indices)
	{
		_indices.push_back(v.x);
		_indices.push_back(v.y);
		_indices.push_back(v.z);
	}

	// Create Vertex Array Object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate Vertex Buffer
	glGenBuffers(1, &_vertexBuffer);
	glGenBuffers(1, &_indexBuffer);

	// Fill Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexPositionNormal), _vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(uint), _indices.data(), GL_STATIC_DRAW);

	// Set Vertex Attributes
	glEnableVertexAttribArray(attribute_position);
	glVertexAttribPointer(attribute_position, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)0);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(attribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)(0 + sizeof(vec3)));

	glBindVertexArray(0);

	debugGLError();
}

void Cylinder::Render()
{
	Shape::Render();

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
#pragma endregion

#pragma region PYRAMID


Pyramid::Pyramid(vec4 color)
{
	_vertexBuffer = _indexBuffer = BAD_BUFFER;

	_color = color;

	VertexPositionNormal vertices[18] = {
		{ vec3(0, 0, 0), vec3(0, -1, 0) },
		{ vec3(1, 0, 0), vec3(0, -1, 0) },
		{ vec3(0, 0, 1), vec3(0, -1, 0) },

		{ vec3(0, 0, 1), vec3(0, -1, 0) },
		{ vec3(1, 0, 0), vec3(0, -1, 0) },
		{ vec3(1, 0, 1), vec3(0, -1, 0) },


		{ vec3( 0, 0,  0), normalize(vec3(0, .5, -1)) },
		{ vec3( 1, 0,  0), normalize(vec3(0, .5, -1)) },
		{ vec3(.5, 1, .5), normalize(vec3(0, .5, -1)) },

		{ vec3( 0, 0,  0), normalize(vec3(-1, .5, 0)) },
		{ vec3( 0, 0,  1), normalize(vec3(-1, .5, 0)) },
		{ vec3(.5, 1, .5), normalize(vec3(-1, .5, 0)) },

		{ vec3( 0, 0,  1), normalize(vec3(0, .5, 1)) },
		{ vec3( 1, 0,  1), normalize(vec3(0, .5, 1)) },
		{ vec3(.5, 1, .5), normalize(vec3(0, .5, 1)) },

		{ vec3( 1, 0,  0), normalize(vec3(1, .5, 0)) },
		{ vec3( 1, 0,  1), normalize(vec3(1, .5, 0)) },
		{ vec3(.5, 1, .5), normalize(vec3(1, .5, 0)) }
	};
	_vertices.assign(&vertices[0], &vertices[18]);

	for (uint x = 0; x < 18; x++)
		_vertices[x].position -= vec3(.5, 0, .5);

	// Create Vertex Array Object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Generate Vertex Buffer
	glGenBuffers(1, &_vertexBuffer);

	// Fill Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexPositionNormal), _vertices.data(), GL_STATIC_DRAW);

	// Set Vertex Attributes
	glEnableVertexAttribArray(attribute_position);
	glVertexAttribPointer(attribute_position, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)0);
	glEnableVertexAttribArray(attribute_normal);
	glVertexAttribPointer(attribute_normal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormal), (const GLvoid*)(0 + sizeof(vec3)));

	glBindVertexArray(0);

	debugGLError();
}

void Pyramid::Render()
{
	Shape::Render();

	glBindVertexArray(_vao);

	glDrawArrays(GL_TRIANGLES, 0, 18);

	glBindVertexArray(0);
}
#pragma endregion
