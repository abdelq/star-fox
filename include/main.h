#pragma once

#pragma warning(push, 0)
// Library Defines
#define GLEW_STATIC
#define GLM_FORCE_RADIANS

#ifndef _MSC_VER
#if __GNUC__ != 4 || __GNUC_MINOR__ < 7
#define override
#define nullptr NULL
#endif
#endif

// Core Library Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// SL/STL/C Includes
#include <climits>
#include <vector>
#include <memory>
#pragma warning(pop)

// Additional Includes
#include <log.h>

// Utility Defines
#define abstract = 0
#define BAD_BUFFER UINT_MAX

typedef glm::float_t decimal;

#ifndef _MSC_VER
#if __GNUC__ != 4 || __GNUC_MINOR__ < 7
#define override
#define nullptr NULL
#endif
#include <strings.h>
#define _stricmp strcasecmp
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1700 || !defined(_MSC_VER)
namespace std
{
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique(Args&&... args) {
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1600
typedef unsigned char uint8_t;
#include <float.h>
#define INFINITY 1e+300 * 1e+300
namespace std
{
	inline bool isfinite(double d)
	{
		return _finite(d) != 0;
	}
};
#endif

// Misc Functions
void onGLFWError(int code, const char* error);
void debugGLError();
GLuint loadShader(const char* filename, GLuint shader_type);
