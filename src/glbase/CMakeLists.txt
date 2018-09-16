link_libraries(glfw ${OPENGL_glu_LIBRARY})

link_libraries(${GLBASE_LIBRARIES})

include_directories(${GLBASE_SOURCE_DIR}/../include)
				 
file(GLOB SOURCE "*.cpp")
file(GLOB HEADERS "*.h")

set(EXT_HEADERS ${GLBASE_SOURCE_DIR}/../include/main.h ${GLBASE_SOURCE_DIR}/../include/log.h)

add_executable(glbase ${SOURCE} ${HEADERS} ${EXT_HEADERS})
target_link_libraries(glbase glfw glew ${OPENGL_LIBRARY} ${X11_LIBRARY})