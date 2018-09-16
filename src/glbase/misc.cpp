#include <main.h>

#include <GL/glu.h>
#include <fstream>

void debugGLError()
{
	GLenum e = glGetError();
	if (e == GL_NO_ERROR)
		return;
	else
	{
		_LOG_CRIT() << "OpenGL Error" << e << ": " << gluErrorString(e) << std::endl;
	}
}

void onGLFWError(int code, const char* error)
{
	_LOG_ERR() << "GLFW Error " << code << ":" << error;
}

GLuint loadShader(const char* filename, GLuint shader_type)
{
	GLint compile_ok = GL_FALSE;

	std::ifstream is(filename);
	if (is.bad() || !is.is_open())
	{
        is.open((std::string("../") + filename).c_str());
		if (is.bad() || !is.is_open())
		{
			_LOG_CRIT() << "Could not open shader file '" << filename << "'." << std::endl;
		}
	}

	std::string shader_source;

	is.seekg(0, is.end);
	shader_source.reserve((unsigned int)is.tellg());
	is.seekg(0, is.beg);

	shader_source.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

	const GLchar* p_shader_source = shader_source.c_str();

	GLuint s = glCreateShader(shader_type);
	glShaderSource(s, 1, &p_shader_source, 0);
	glCompileShader(s);
	glGetShaderiv(s, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok)
	{
		GLint maxLength = 0;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength == 0)
		{
			_LOG_CRIT() << "Could not compile shader '" << filename << "': No errors reported." << std::endl;
		}
		else
		{
			GLchar* shader_error = new GLchar[maxLength];
			glGetShaderInfoLog(s, maxLength, &maxLength, shader_error);
			_LOG_CRIT() << "Could not compile shader '" << filename << "':" << std::endl << shader_error << std::endl;
		}
	}

	debugGLError();

	return s;
}
