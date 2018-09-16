include_directories(${GLBASE_SOURCE_DIR}/../include)

file(GLOB SOURCE "*.c")
file(GLOB HEADERS "*.h")

add_library(glew ${SOURCE} ${HEADERS})

