# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.8)
project ("JustCtrl")

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

# Check if the example argument is set
if(DEFINED example)
  # Add the value of the example argument to the project_source_files list
  set(project_source_files ${project_source_files} ${example})
else()
  # Use main.cpp as the default source file
  set(project_source_files ${project_source_files} "source/main.cpp")
endif()

file(GLOB project_include_files
	"include/textbox.h"
	"include/propertylist.h"
	"include/tabwindow.h"
	"include/label.h"
	"include/checkbox.h"
	"include/radiobutton.h"
	"include/JustCtrl.h"
)

file(GLOB source_files
	"source/textbox.cpp"
	"source/propertylist.cpp"
	"source/tabwindow.cpp"
	"source/label.cpp"
	"source/checkbox.cpp"
	"source/radiobutton.cpp"
	"source/JustCtrl.cpp"
)

set(project_source_files ${project_source_files} ${source_files})
add_executable(JustCtrl WIN32 ${project_include_files} ${project_source_files} ${APP_RESOURCE_FILE})

target_include_directories(JustCtrl PRIVATE "include")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT JustCtrl)
set_target_properties(JustCtrl PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
