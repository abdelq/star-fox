
include_directories(${GLBASE_SOURCE_DIR}/glfw
					${GLBASE_BINARY_DIR}/glfw
					${GLBASE_SOURCE_DIR}/../include
                    ${GLBASE_INCLUDE_DIRS})

set(common_HEADERS ${GLBASE_BINARY_DIR}/glfw/config.h internal.h
                   ${GLBASE_SOURCE_DIR}/../include/GLFW/glfw3.h
                   ${GLBASE_SOURCE_DIR}/../include/GLFW/glfw3native.h)
set(common_SOURCES clipboard.c context.c gamma.c init.c input.c joystick.c
                   monitor.c time.c window.c)

set(glfw_HEADERS ${common_HEADERS} x11_platform.h)
set(glfw_SOURCES ${common_SOURCES} x11_clipboard.c x11_gamma.c x11_init.c
				 x11_joystick.c x11_monitor.c x11_time.c x11_window.c
				 x11_unicode.c)

list(APPEND glfw_HEADERS ${common_HEADERS} glx_platform.h)
list(APPEND glfw_SOURCES ${common_SOURCES} glx_context.c)

add_library(glfw ${glfw_SOURCES} ${glfw_HEADERS})
set_target_properties(glfw PROPERTIES OUTPUT_NAME "${GLFW_LIB_NAME}")

if (GLFW_INSTALL)
    install(TARGETS glfw EXPORT glfwTargets DESTINATION lib${LIB_SUFFIX})
endif()

