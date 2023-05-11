#--------------------------------------------------------------------------
# podracer project
#--------------------------------------------------------------------------

PROJECT(podracer)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SET(files_proto)
#flat_compile(
#	test.fbs
#	# add more flatbuffer files here
#) 

SOURCE_GROUP("podracer" FILES ${files_project})

ADD_EXECUTABLE(podracer ${files_project} ${files_proto})
target_include_directories(podracer PRIVATE "${CMAKE_BINARY_DIR}/generated/flat")

TARGET_LINK_LIBRARIES(podracer core render)
ADD_DEPENDENCIES(podracer core render)

IF(MSVC)
    set_property(TARGET podracer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()