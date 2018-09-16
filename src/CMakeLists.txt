project(GLBASE C CXX)

cmake_minimum_required(VERSION 2.8)

option(GLBASE_INSTALL "Generate installation target" ON)

find_package(OpenGL REQUIRED)

find_package(Threads REQUIRED)

set(CXX_NORM_CXX98 1)   # C++98
set(CXX_NORM_CXX03 2)   # C++03
set(CXX_NORM_CXX11 3)   # C++11

# - Set the wanted C++ norm
# Adds the good argument to the command line in function of the compiler
# Currently only works with g++ and clang++
macro(set_cxx_norm NORM)

    # Extract c++ compiler --version output
    exec_program(
        ${CMAKE_CXX_COMPILER}
        ARGS --version
        OUTPUT_VARIABLE _compiler_output
    )
    # Keep only the first line
    string(REGEX REPLACE
        "(\n.*$)"
        ""
        cxx_compiler_version "${_compiler_output}"
    )
    # Extract the version number
    string(REGEX REPLACE
        "([^0-9.])|([0-9.][^0-9.])"
        ""
        cxx_compiler_version "${cxx_compiler_version}"
    )

    if(CMAKE_COMPILER_IS_GNUCXX)

        if(${NORM} EQUAL ${CXX_NORM_CXX98})
            add_definitions("-std=c++98")
        elseif(${NORM} EQUAL ${CXX_NORM_CXX03})
            add_definitions("-std=c++03")
        elseif(${NORM} EQUAL ${CXX_NORM_CXX11})
            if(${cxx_compiler_version} VERSION_LESS "4.7.0")
                add_definitions("-std=c++0x")
            else()
                add_definitions("-std=c++11")
            endif()
        endif()

    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")

        if(${NORM} EQUAL ${CXX_NORM_CXX11})
            add_definitions("-std=c++0x")
        endif()

    endif()

endmacro()

set(_GLFW_USE_OPENGL 1)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas")
set_cxx_norm(CXX_NORM_CXX11)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g -D_DEBUG")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_BUILD}../../../build/${CMAKE_BUILD_TYPE}/)

#--------------------------------------------------------------------
# Use X11 for window creation
#--------------------------------------------------------------------

find_package(X11 REQUIRED)
set(_GLFW_X11 1)

set(GLBASE_PKG_DEPS "${GLBASE_PKG_DEPS} x11")

# Set up library and include paths
list(APPEND GLBASE_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
list(APPEND GLBASE_LIBRARIES ${X11_X11_LIB} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX AND NOT APPLE)
        list(APPEND GLBASE_LIBRARIES ${RT_LIBRARY})
endif()

# Check for XRandR (modern resolution switching and gamma control)
if (NOT X11_Xrandr_FOUND)
	message(FATAL_ERROR "The RandR library and headers were not found")
endif()

list(APPEND GLBASE_INCLUDE_DIRS ${X11_Xrandr_INCLUDE_PATH})
list(APPEND GLBASE_LIBRARIES ${X11_Xrandr_LIB})
set(GLBASE_PKG_DEPS "${GLBASE_PKG_DEPS} xrandr")

# Check for XInput (high-resolution cursor motion)
if (NOT X11_Xinput_FOUND)
	message(FATAL_ERROR "The XInput library and headers were not found")
endif()

list(APPEND GLBASE_INCLUDE_DIRS ${X11_Xinput_INCLUDE_PATH})

if (X11_Xinput_LIB)
        list(APPEND GLBASE_LIBRARIES ${X11_Xinput_LIB})
else()
	# Backwards compatibility (bug in CMake 2.8.7)
        list(APPEND GLBASE_LIBRARIES Xi)
endif()
set(GLBASE_PKG_DEPS "${GLBASE_PKG_DEPS} xi")

# Check for Xf86VidMode (fallback gamma control)
if (NOT X11_xf86vmode_FOUND)
	message(FATAL_ERROR "The Xf86VidMode library and headers were not found")
endif()

list(APPEND GLBASE_INCLUDE_DIRS ${X11_xf86vmode_INCLUDE_PATH})
set(GLBASE_PKG_DEPS "${GLBASE_PKG_DEPS} xxf86vm")

if (X11_Xxf86vm_LIB)
        list(APPEND GLBASE_LIBRARIES ${X11_Xxf86vm_LIB})
else()
	# Backwards compatibility (see CMake bug 0006976)
        list(APPEND GLBASE_LIBRARIES Xxf86vm)
endif()

# Check for Xkb (X keyboard extension)
if (NOT X11_Xkb_FOUND)
	message(FATAL_ERROR "The X keyboard extension headers were not found")
endif() 

list(APPEND GLBASE_INCLUDE_DIR ${X11_Xkb_INCLUDE_PATH})

find_library(RT_LIBRARY rt)
mark_as_advanced(RT_LIBRARY)
if (RT_LIBRARY)
        list(APPEND GLBASE_LIBRARIES ${RT_LIBRARY})
        set(GLBASE_PKG_LIBS "${GLBASE_PKG_LIBS} -lrt")
endif()

find_library(MATH_LIBRARY m)
mark_as_advanced(MATH_LIBRARY)
if (MATH_LIBRARY)
        list(APPEND GLBASE_LIBRARIES ${MATH_LIBRARY})
        set(GLBASE_PKG_LIBS "${GLBASE_PKG_LIBS} -lm")
endif()

#--------------------------------------------------------------------
# Use GLX for context creation
#--------------------------------------------------------------------

set(_GLFW_GLX 1)

list(APPEND GLBASE_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND GLBASE_LIBRARIES ${OPENGL_gl_LIBRARY})

set(GLBASE_PKG_DEPS "${GLBASE_PKG_DEPS} gl")

include(CheckFunctionExists)

set(CMAKE_REQUIRED_LIBRARIES ${OPENGL_gl_LIBRARY})
check_function_exists(glXGetProcAddress _GLFW_HAS_GLXGETPROCADDRESS)
check_function_exists(glXGetProcAddressARB _GLFW_HAS_GLXGETPROCADDRESSARB)
check_function_exists(glXGetProcAddressEXT _GLFW_HAS_GLXGETPROCADDRESSEXT)

if (NOT _GLFW_HAS_GLXGETPROCADDRESS AND
	NOT _GLFW_HAS_GLXGETPROCADDRESSARB AND
	NOT _GLFW_HAS_GLXGETPROCADDRESSEXT)
	message(WARNING "No glXGetProcAddressXXX variant found")

	# Check for dlopen support as a fallback

	find_library(DL_LIBRARY dl)
	mark_as_advanced(DL_LIBRARY)
	if (DL_LIBRARY)
		set(CMAKE_REQUIRED_LIBRARIES ${DL_LIBRARY})
	else()
		set(CMAKE_REQUIRED_LIBRARIES "")
	endif()

	check_function_exists(dlopen _GLFW_HAS_DLOPEN)

	if (NOT _GLFW_HAS_DLOPEN)
		message(FATAL_ERROR "No entry point retrieval mechanism found")
	endif()

	if (DL_LIBRARY)
                list(APPEND GLBASE_LIBRARIES ${DL_LIBRARY})
                set(GLBASE_PKG_LIBS "${GLBASE_PKG_LIBS} -ldl")
	endif()
endif()

#--------------------------------------------------------------------
# Export GLBASE library dependencies
#--------------------------------------------------------------------

set(GLBASE_LIBRARIES ${GLBASE_LIBRARIES} CACHE STRING "Dependencies of GLFW")

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------

add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(glbase)

#--------------------------------------------------------------------
# Create generated files
#--------------------------------------------------------------------

configure_file(${GLBASE_SOURCE_DIR}/glfw/config.h.in
               ${GLBASE_BINARY_DIR}/glfw/config.h @ONLY)

configure_file(${GLBASE_SOURCE_DIR}/glfw/glfwConfig.cmake.in
               ${GLBASE_BINARY_DIR}/glfw/glfwConfig.cmake @ONLY)

configure_file(${GLBASE_SOURCE_DIR}/glfw/glfwConfigVersion.cmake.in
               ${GLBASE_BINARY_DIR}/glfw/glfwConfigVersion.cmake @ONLY)

configure_file(${GLBASE_SOURCE_DIR}/glfw/glfw3.pc.in
               ${GLBASE_BINARY_DIR}/glfw/glfw3.pc @ONLY)
