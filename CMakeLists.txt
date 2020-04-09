cmake_minimum_required(VERSION 2.8)

###

project(MyOperator)

set(OPERATOR_TYPE_CHOP 1)
set(OPERATOR_TYPE_TOP 0)
set(OPERATOR_TYPE_SOP 0)

include_directories(
    "src"
    "derivative"
    "derivative/GL"
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib
)

set(LINK_LIBRARIES
)

file(GLOB SRC
    "src/*.cpp"
    "src/*.c"
    "derivative/GL/*.c"
)

###

if (APPLE)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})

###

set(OPERATOR_NAME ${PROJECT_NAME})

add_library(${OPERATOR_NAME} MODULE ${SRC})

set_target_properties(${OPERATOR_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${OPERATOR_NAME} PROPERTIES PREFIX "")
set_target_properties(${OPERATOR_NAME} PROPERTIES BUNDLE TRUE)

set_target_properties(${OPERATOR_NAME} PROPERTIES COMPILE_FLAGS "-DOPERATOR_NAME=${OPERATOR_NAME} -DOPERATOR_TYPE_CHOP=${OPERATOR_TYPE_CHOP} -DOPERATOR_TYPE_TOP=${OPERATOR_TYPE_TOP} -DOPERATOR_TYPE_SOP=${OPERATOR_TYPE_SOP}")

find_package(OpenGL REQUIRED)
target_link_libraries(${OPERATOR_NAME} ${OPENGL_LIBRARY} ${LINK_LIBRARIES})

set_target_properties(${OPERATOR_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
set_target_properties(${OPERATOR_NAME} PROPERTIES VS_DEBUGGER_COMMAND "C:/Program Files/Derivative/TouchDesigner099/bin/TouchDesigner099.exe")
set_target_properties(${OPERATOR_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "test.toe")

### hot-reloading using OSC message (`oscer` https://github.com/aike/oscer)

if (APPLE)
    set(OSCER "${CMAKE_CURRENT_SOURCE_DIR}/bin/MacOSX/oscer")
elseif(WIN32)
    set(OSCER "${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows64/oscer.exe")
endif()

add_custom_command(TARGET ${OPERATOR_NAME} PRE_BUILD COMMAND ${OSCER} ARGS localhost 23423 /unload)
add_custom_command(TARGET ${OPERATOR_NAME} POST_BUILD COMMAND ${OSCER} ARGS localhost 23423 /load)
