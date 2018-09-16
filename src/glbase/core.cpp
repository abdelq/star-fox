#include "core.h"
#include "scene.h"
#include <iostream>

Core::Core() : _window(nullptr), _textVertexBuffer(BAD_BUFFER), _textUVBuffer(BAD_BUFFER), _attribute_textPosition(4), _attribute_textUV(3), _width(768), _height(640)
{
	GLFWInit();
	GLEWInit();

	// Clear possible error from GLFW/GLEW initialization
	glGetError();

	TextInit();
	LineInit();

	CoreInit();

	_callback_object = this;

	glfwSetMouseButtonCallback(_window, MouseClickCallback);
	glfwSetCursorPosCallback(_window, MouseMoveCallback);
	glfwSetScrollCallback(_window, MouseScrollCallback);
	glfwSetKeyCallback(_window, KeyCallback);

	_LOG_INFO() << "Main initialization sequence completed.";
}

Core::~Core()
{
	glDeleteProgram(_shaderProgram);
	glDeleteProgram(_textProgram);

	if (_textVertexBuffer != BAD_BUFFER)
		glDeleteBuffers(1, &_textVertexBuffer);

	if (_textUVBuffer != BAD_BUFFER)
		glDeleteBuffers(1, &_textUVBuffer);

	glDeleteVertexArrays(1, &_textVAO);

	glfwTerminate();
}

void Core::Run()
{
	double time = glfwGetTime();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(_window))
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram);
		glUniformMatrix4fv(_uniform_projectionMatrix, 1, GL_FALSE, glm::value_ptr(_projectionMatrix));
		glUniformMatrix4fv(_uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(_viewMatrix));

		double ntime = glfwGetTime();

		Render(ntime - time);

		time = ntime;

		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}

void Core::GLFWInit()
{
	// Set GLFW error callback
	glfwSetErrorCallback(onGLFWError);

	// Initialize window manager first
	if (!glfwInit())
	{
		_LOG_CRIT() << "GLFW initialization failed!";
	}

	_LOG_INFO() << "GLFW initialized.";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	_window = glfwCreateWindow(_width, _height, "IFT3355", NULL, NULL);

	if (!_window)
	{
		glfwTerminate();
		_LOG_CRIT() << "GLFW: Could not create window!";
	}

	_LOG_INFO() << "Window created successfully.";

	glfwMakeContextCurrent(_window);
}

void Core::GLEWInit()
{
	glewExperimental = GL_TRUE;
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		_LOG_CRIT() << "GLEW initialization failed (code " << glew_error << "):" << glewGetErrorString(glew_error);
	}

	if (!glewIsSupported("GL_VERSION_3_2") && (!glewIsSupported("GL_ARB_vertex_buffer_object") || !glewIsSupported("GL_ARB_vertex_array_object")))
	{
		_LOG_CRIT() << "ARB_vertex_buffer_object not supported!";
	}

	_LOG_INFO() << "GLEW initialized.";
}

void Core::TextInit()
{
	GLint link_ok = GL_FALSE;

	_textProgram = glCreateProgram();

	GLuint vs = loadShader("../shaders/text_vertex.glsl", GL_VERTEX_SHADER);
	GLuint fs = loadShader("../shaders/text_fragment.glsl", GL_FRAGMENT_SHADER);

	glAttachShader(_textProgram, vs);
	glAttachShader(_textProgram, fs);

	glBindAttribLocation(_textProgram, _attribute_textPosition, "in_position");
	glBindAttribLocation(_textProgram, _attribute_textUV, "in_uv");

	glLinkProgram(_textProgram);
	glGetProgramiv(_textProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		GLint maxLength = 0;
		glGetProgramiv(_textProgram, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
		{
			_LOG_CRIT() << "Could not link text shader: No errors reported." << std::endl;
		}

		{
			GLchar* link_error = new GLchar[(unsigned int)maxLength];
			glGetProgramInfoLog(_textProgram, maxLength, &maxLength, link_error);
			_LOG_CRIT() << "Could not link text shader: " << std::endl << link_error << std::endl;
		}
	}

	_uniform_textSampler = glGetUniformLocation(_textProgram, "sampler");
	_uniform_textColor = glGetUniformLocation(_textProgram, "color");

	glGenBuffers(1, &_textVertexBuffer);
	glGenBuffers(1, &_textUVBuffer);

	// Create Vertex Array Object
	glGenVertexArrays(1, &_textVAO);
	glBindVertexArray(_textVAO);

	glEnableVertexAttribArray(_attribute_textPosition);
	glBindBuffer(GL_ARRAY_BUFFER, _textVertexBuffer);
	glVertexAttribPointer(_attribute_textPosition, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glEnableVertexAttribArray(_attribute_textUV);
	glBindBuffer(GL_ARRAY_BUFFER, _textUVBuffer);
	glVertexAttribPointer(_attribute_textUV, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glBindVertexArray(0);

	_fontTexture = std::unique_ptr<Texture>(new Texture("../consolas.tga"));

	debugGLError();
}

void Core::CoreInit()
{
	GLint link_ok = GL_FALSE;

	_shaderProgram = glCreateProgram();

	GLuint vs = loadShader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
	GLuint fs = loadShader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);

	glAttachShader(_shaderProgram, vs);
	glAttachShader(_shaderProgram, fs);

	Shape::InitializePreLink(_shaderProgram);
	glLinkProgram(_shaderProgram);
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		GLint maxLength = 0;
		glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
		{
			_LOG_CRIT() << "Could not link primary shader: No errors reported." << std::endl;
		}

		{
			GLchar* link_error = new GLchar[(unsigned int)maxLength];
			glGetProgramInfoLog(_shaderProgram, maxLength, &maxLength, link_error);
			_LOG_CRIT() << "Could not link primary shader: " << std::endl << link_error << std::endl;
		}
	}

	Shape::InitializePostLink(_shaderProgram);

	_uniform_projectionMatrix = glGetUniformLocation(_shaderProgram, "projection");
	_uniform_viewMatrix = glGetUniformLocation(_shaderProgram, "view");

	_projectionMatrix = glm::perspective(radians(45.0f), decimal(_width) / decimal(_height), 0.1f, 1000.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_FRONT_AND_BACK);

	debugGLError();
}

void Core::DrawText(const char* text, glm::vec2 position, const glm::vec4 &color, unsigned int pixel_size, TextAlign align)
{
	glBindVertexArray(0);

	int width, height;
	glfwGetWindowSize(_window, &width, &height);

	float letter_w = pixel_size / (float)width;
	float letter_h = pixel_size / (float)height * 2;

	unsigned int length = strlen(text);

	float text_width = length * letter_w;
	if (align == TextAlign::ALIGN_RIGHT)
		position.x -= text_width;
	if (align == TextAlign::ALIGN_CENTER)
		position.x -= text_width * 0.5f;

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> uv;

	for (unsigned int i = 0; i < length; i++)
	{
		glm::vec2 vertex_up_left = glm::vec2(position.x + i*letter_w, position.y + letter_h);
		glm::vec2 vertex_up_right = glm::vec2(position.x + i*letter_w + letter_w, position.y + letter_h);
		glm::vec2 vertex_down_right = glm::vec2(position.x + i*letter_w + letter_w, position.y);
		glm::vec2 vertex_down_left = glm::vec2(position.x + i*letter_w, position.y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		const float xd = 16.0f, yd = 8.0f;

		char character = text[i] - '!' + 1;
		float uv_x = (character % 16) / xd;
		float uv_y = (character / 16) / yd;

		glm::vec2 uv_up_left = glm::vec2(uv_x, 1.0f - uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / xd, 1.0f - uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / xd, 1.0f - (uv_y + 1.0f / yd));
		glm::vec2 uv_down_left = glm::vec2(uv_x, 1.0f - (uv_y + 1.0f / yd));
		uv.push_back(uv_up_left);
		uv.push_back(uv_down_left);
		uv.push_back(uv_up_right);

		uv.push_back(uv_down_right);
		uv.push_back(uv_up_right);
		uv.push_back(uv_down_left);
	}

	// Bind shader
	glUseProgram(_textProgram);

	glUniform4fv(_uniform_textColor, 1, glm::value_ptr(color));

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _fontTexture->glID());
	glUniform1i(_uniform_textSampler, 0);

	glBindVertexArray(_textVAO);

	//glEnableVertexAttribArray(_attribute_textPosition);
	glBindBuffer(GL_ARRAY_BUFFER, _textVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(_attribute_textPosition, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	//glEnableVertexAttribArray(_attribute_textUV);
	glBindBuffer(GL_ARRAY_BUFFER, _textUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
	//glVertexAttribPointer(_attribute_textUV, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glBindVertexArray(0);
}

//based on https://github.com/glampert/debug-draw/blob/master/debug_draw.hpp
void Core::AABB(glm::vec3 min, glm::vec3 max)
{
	vec3 dim = max - min;
	vec3 points[8];
	vec3 bb[2] = { min,max };

	//generate points
	for (int i = 0; i < 8; ++i)
	{
		points[i].x = bb[(i ^ (i >> 1)) & 1].x;
		points[i].y = bb[(i >> 1) & 1].y;
		points[i].z = bb[(i >> 2) & 1].z;
	}

	//box generation
	for (int i = 0; i < 4; ++i)
	{
		_lineVertices.push_back(points[i]);
		_lineVertices.push_back(points[(i + 1) & 3]);
		_lineVertices.push_back(points[4 + i]);
		_lineVertices.push_back(points[4 + ((i + 1) & 3)]);
		_lineVertices.push_back(points[i]);
		_lineVertices.push_back(points[4 + i]);
	}
}

#define MAX_LINE 4096

void Core::LineInit()
{
	GLint link_ok = GL_FALSE;

	_lineShaderProgram = glCreateProgram();

	GLuint vs = loadShader("../shaders/line_vertex.glsl", GL_VERTEX_SHADER);
	GLuint fs = loadShader("../shaders/line_fragment.glsl", GL_FRAGMENT_SHADER);

	glAttachShader(_lineShaderProgram, vs);
	glAttachShader(_lineShaderProgram, fs);

	glBindAttribLocation(_lineShaderProgram, 0, "in_position");
	glLinkProgram(_lineShaderProgram);
	glGetProgramiv(_lineShaderProgram, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		GLint maxLength = 0;
		glGetProgramiv(_lineShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
		{
			_LOG_CRIT() << "Could not link primary shader: No errors reported." << std::endl;
		}

		{
			GLchar* link_error = new GLchar[(unsigned int)maxLength];
			glGetProgramInfoLog(_lineShaderProgram, maxLength, &maxLength, link_error);
			_LOG_CRIT() << "Could not link primary shader: " << std::endl << link_error << std::endl;
		}
	}

	glBindVertexArray(0);
	glGenVertexArrays(1, &_lineVAO);

	glBindVertexArray(_lineVAO);

	glGenBuffers(1, &_lineVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _lineVertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, MAX_LINE * 3 * 2 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));


	glBindVertexArray(0);

	debugGLError();


}

void Core::DrawAABBs()
{
	glUseProgram(_lineShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_lineShaderProgram, "projection"), 1, GL_FALSE, value_ptr(_projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(_lineShaderProgram, "view"), 1, GL_FALSE, value_ptr(_viewMatrix));

	glBindVertexArray(_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _lineVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _lineVertices.size() * sizeof(glm::vec3), &_lineVertices[0], GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINES, 0, _lineVertices.size());

	debugGLError();
	glBindVertexArray(0);
}

// Input callbacks

Core* Core::_callback_object = nullptr;

void Core::MouseClickCallback(GLFWwindow* /*w*/, int button, int action, int /*modifiers*/)
{
	bool down = action == GLFW_PRESS;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		_callback_object->OnMouseLeft(down);
		return;
	case GLFW_MOUSE_BUTTON_RIGHT:
		_callback_object->OnMouseRight(down);
		return;
	}
}

void Core::MouseScrollCallback(GLFWwindow* /*w*/, double dx, double dy)
{
	_callback_object->OnMouseWheel(dx, dy);
}

void Core::MouseMoveCallback(GLFWwindow* /*w*/, double x, double y)
{
	_callback_object->OnMouseMove((float)x, (float)y);
}

void Core::KeyCallback(GLFWwindow* /*w*/, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (action == GLFW_REPEAT)
		return;

	bool down = action == GLFW_PRESS;
	switch (key)
	{
	case GLFW_KEY_W:
		_callback_object->OnKeyW(down);
		return;
	case GLFW_KEY_Z:
		_callback_object->OnKeyW(down);
		return;
	case GLFW_KEY_A:
		_callback_object->OnKeyA(down);
		return;
	case GLFW_KEY_Q:
		_callback_object->OnKeyA(down);
		return;
	case GLFW_KEY_S:
		_callback_object->OnKeyS(down);
		return;
	case GLFW_KEY_D:
		_callback_object->OnKeyD(down);
		return;
	case GLFW_KEY_E:
		_callback_object->OnKeyE(down);
		return;
	case GLFW_KEY_SPACE:
		_callback_object->OnKeySPACE(down);
		return;
	case GLFW_KEY_TAB:
		_callback_object->OnKeyTAB(down);
		return;
	}
}