# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.8)
project ("JustCtrl")

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

file(GLOB project_include_files
	"include/label.h"
	"include/checkbox.h"
	"include/radiobutton.h"
	"include/JustCtrl.h"
)

file(GLOB project_source_files
	"source/label.cpp"
	"source/checkbox.cpp"
	"source/radiobutton.cpp"
	"source/JustCtrl.cpp"
	"source/main.cpp"
)

add_executable(JustCtrl WIN32 ${project_include_files} ${project_source_files} ${APP_RESOURCE_FILE})

target_include_directories(JustCtrl PRIVATE "include")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT JustCtrl)
set_target_properties(JustCtrl PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
