cmake_minimum_required(VERSION 2.8)

set(TOUCHDESIGNER_EXE "C:/Program Files/Derivative/TouchDesigner/bin/TouchDesigner.exe" CACHE FILEPATH "TouchDesigner.exe Path")

###

project(CustomOps)

include_directories(
    "src"
    "derivative"
    "derivative/GL"
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib
)

file(GLOB DEP_SRC
    "derivative/GL/*.c"
    "derivative/*.cpp"
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

function(BuiltCustomOp OPNAME SRC LIBRARIES)

    add_library(${OPNAME} MODULE ${SRC} ${DEP_SRC})

    set_target_properties(${OPNAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${OPNAME} PROPERTIES PREFIX "")
    set_target_properties(${OPNAME} PROPERTIES BUNDLE TRUE)

    find_package(OpenGL REQUIRED)
    target_link_libraries(${OPNAME} ${OPENGL_LIBRARY} ${LINK_LIBRARIES})

    set(OPTEST ${OPNAME}_TEST)
    add_custom_target(${OPTEST})

    set_target_properties(${OPTEST} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)
    set_target_properties(${OPTEST} PROPERTIES VS_DEBUGGER_COMMAND ${TOUCHDESIGNER_EXE})
    set_target_properties(${OPTEST} PROPERTIES VS_DEBUGGER_ENVIRONMENT "TOUCH_TEXT_CONSOLE=1")
    set_target_properties(${OPTEST} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "test.toe")

    ### hot-reloading using OSC message (`oscer` https://github.com/aike/oscer)

    if (APPLE)
        set(OSCER "${CMAKE_CURRENT_SOURCE_DIR}/bin/MacOSX/oscer")
    elseif(WIN32)
        set(OSCER "${CMAKE_CURRENT_SOURCE_DIR}/bin/Windows64/oscer.exe")
    endif()

    add_custom_command(TARGET ${OPTEST} PRE_BUILD COMMAND ${OSCER} ARGS localhost 23423 /unload)
    add_custom_command(TARGET ${OPTEST} POST_BUILD COMMAND ${OSCER} ARGS localhost 23423 /load)

endfunction(BuiltCustomOp)

###

BuiltCustomOp(CustomCHOP "src/CHOP_main.cpp" "")
BuiltCustomOp(CustomSOP "src/SOP_main.cpp" "")
BuiltCustomOp(CustomTOP "src/TOP_main.cpp" "")
BuiltCustomOp(CustomDAT "src/DAT_main.cpp" "")