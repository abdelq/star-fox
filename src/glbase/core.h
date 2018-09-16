#include <main.h>
#include "texture.h"

class Core
{
public:
	Core();
	virtual ~Core();

	void Run();

protected:
	virtual void Render(double dt) abstract;

	virtual void OnKeyW(bool down) { _LOG_INFO() << "W " << (down ? "down." : "up.") << std::endl; }
	virtual void OnKeyS(bool down) { _LOG_INFO() << "S " << (down ? "down." : "up.") << std::endl; }
	virtual void OnKeyA(bool down) { _LOG_INFO() << "A " << (down ? "down." : "up.") << std::endl; }
	virtual void OnKeyD(bool down) { _LOG_INFO() << "D " << (down ? "down." : "up.") << std::endl; }
	virtual void OnKeyE(bool down) { _LOG_INFO() << "E " << (down ? "down." : "up.") << std::endl; }

	virtual void OnKeyTAB(bool down) { _LOG_INFO() << "TAB " << (down ? "down." : "up.") << std::endl; }
	virtual void OnKeySPACE(bool down) { _LOG_INFO() << "SPACE " << (down ? "down." : "up.") << std::endl; }

	virtual void OnMouseMove(float x, float y) { _LOG_INFO() << "Mouse x=" << x << ", y=" << y << std::endl; }
	virtual void OnMouseLeft(bool down) { _LOG_INFO() << "LMB " << (down ? "down." : "up.") << std::endl; }
	virtual void OnMouseRight(bool down) { _LOG_INFO() << "RMB " << (down ? "down." : "up.") << std::endl; }
	virtual void OnMouseWheel(double dx, double dy) { _LOG_INFO() << "Wheel dx=" << dx << ", dy=" << dy << std::endl; }

	enum TextAlign
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	void DrawText(const char* text, glm::vec2 position, const glm::vec4 &color = glm::vec4(1, 1, 1, 1), unsigned int pixel_size = 32, TextAlign align = ALIGN_LEFT);
	void AABB(glm::vec3 min, glm::vec3 max);
	void DrawAABBs();

private:
	void GLFWInit();
	void GLEWInit();
	void TextInit();
	void CoreInit();
	void LineInit();

	static void MouseClickCallback(GLFWwindow* w, int button, int action, int modifiers);
	static void MouseScrollCallback(GLFWwindow* w, double dx, double dy);
	static void MouseMoveCallback(GLFWwindow* w, double x, double y);
	static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);

	static Core* _callback_object;

protected:
	GLFWwindow* _window;

	GLuint _shaderProgram;

	GLint _uniform_projectionMatrix, _uniform_viewMatrix;

	glm::mat4 _projectionMatrix, _viewMatrix;

	// Line rendering
	GLuint _lineVAO;
	GLuint _lineVertexBuffer;
	std::vector <glm::vec3> _lineVertices;
	GLuint _lineShaderProgram;

	// Text rendering
	GLuint _textVAO;
	GLuint _textVertexBuffer, _textUVBuffer;
	GLuint _textProgram;

	GLint _uniform_textSampler, _uniform_textColor;
	GLint _attribute_textPosition, _attribute_textUV;

	std::unique_ptr<Texture> _fontTexture;

	glm::uint _width, _height;
};