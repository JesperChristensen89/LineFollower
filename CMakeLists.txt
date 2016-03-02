#####################################
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "/usr/local/lib/cmake/${CMAKE_MODULE_PATH}")
project( line_detection )
find_package(raspicam REQUIRED)
find_package( OpenCV )
IF (OpenCV_FOUND AND raspicam_CV_FOUND)

	

	file(GLOB_RECURSE line_SOURCES "src/*.cpp")
	file(GLOB_RECURSE line_HEADERS "src/*.h")

	set (line_INCLUDE_DIRS "")
	foreach (_headerFile ${line_HEADERS})
		get_filename_component(_dir ${_headerFile} PATH)
		list (APPEND line_INCLUDE_DIRS ${_dir})
	endforeach()

	list(REMOVE_DUPLICATES line_INCLUDE_DIRS)

	include_directories( ${OpenCV_INCLUDE_DIRS} ${line_INCLUDE_DIRS} )

	add_executable( line_detection ${line_SOURCES})
	target_link_libraries( line_detection ${raspicam_CV_LIBS})



ELSE()
	MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()
#####################################
