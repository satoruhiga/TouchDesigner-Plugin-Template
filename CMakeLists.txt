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

if (APPLE)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/out)

add_library(${PROJECT_NAME} MODULE ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES BUNDLE TRUE)

add_definitions(-DPROJECT_NAME=${PROJECT_NAME})
add_definitions(-DOPERATOR_TYPE_CHOP=${OPERATOR_TYPE_CHOP})
add_definitions(-DOPERATOR_TYPE_TOP=${OPERATOR_TYPE_TOP})
add_definitions(-DOPERATOR_TYPE_SOP=${OPERATOR_TYPE_SOP})

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY})

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND "C:/Program Files/Derivative/TouchDesigner099/bin/TouchDesigner099.exe")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "test.toe")