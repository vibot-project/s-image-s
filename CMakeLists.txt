project(seoop)
cmake_minimum_required(2.8)

#options
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
option(WARNINGS_AS_ERRORS   "Compiler warnings as errors"       "OFF")
option(OPT_ASAN             "Use adress sanitizer (debug)"      "ON")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#dependencies

#find_package( GTest REQUIRED )
find_package( Qt5Widgets REQUIRED )


#source files
file(GLOB_RECURSE sources *.cpp *.cc)
file(GLOB_RECURSE headers *.h *.hpp)
file(GLOB_RECURSE gui *.ui)

qt5_wrap_ui(gui_design
	${PROJECT_SOURCE_DIR}/gui/mainwindow.ui
	${PROJECT_SOURCE_DIR}/gui/help.ui
)

include_directories(${external_includes})

add_library(gui STATIC
        ${gui_sources}
        ${gui_headers}
)

target_link_libraries(gui ${external_libs})


#libs
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set( external_includes ${Qt5Widgets_INCLUDE_DIRS} )
set( external_libs ${Qt5Widgets_LIBRARIES} "-lpthread")