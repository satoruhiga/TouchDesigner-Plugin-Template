cmake_minimum_required(VERSION 2.8)

###

project(TestCHOP)

set(OPERATOR_TYPE_CHOP 1)
set(OPERATOR_TYPE_TOP 0)
set(OPERATOR_TYPE_SOP 0)

file(GLOB SRC
	"src/*.cpp"
	"src/*.c"
	"derivative/GL/*.c"
)

include_directories(src derivative derivative/GL)

###

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

add_library(${PROJECT_NAME} MODULE ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_definitions(-DPROJECT_NAME=${PROJECT_NAME})
add_definitions(-DOPERATOR_TYPE_CHOP=${OPERATOR_TYPE_CHOP})
add_definitions(-DOPERATOR_TYPE_TOP=${OPERATOR_TYPE_TOP})
add_definitions(-DOPERATOR_TYPE_SOP=${OPERATOR_TYPE_SOP})

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})