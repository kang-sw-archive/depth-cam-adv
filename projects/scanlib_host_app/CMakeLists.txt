cmake_minimum_required(VERSION 2.8)

project(scanlib)
set(CMAKE_CXX_STANDARD 17)

# Core library. Includes protocol.
include_directories(src)


# Core functionalities
set(SCANLIB_DIR src/scanlib)
set(SCANLIB_ARCH_DIR ${SCANLIB_DIR}/arch)
file(GLOB_RECURSE SRC_SCANLIB "${SCANLIB_DIR}/*.cpp" "${SCANLIB_DIR}/*.c")
list(REMOVE_ITEM SRC_SCANLIB "${SCANLIB_DIR}/arch/**")

# Platform specific features
if (WIN32)
	file(GLOB_RECURSE PLATFORM 
		"${SCANLIB_ARCH_DIR}/win32/*.cpp" 
		"${SCANLIB_ARCH_DIR}/win32/*.c")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
elseif(UNIX)
	file(GLOB_RECURSE PLATFORM 
		"${SCANLIB_ARCH_DIR}/linux/*.cpp" 
		"${SCANLIB_ARCH_DIR}/linux/*.c")
endif()

# build
add_library(scanlib STATIC ${SRC_PLATFORM} ${SRC_SCANLIB})
add_dependencies(scanlib nana)

# -- for GUI app
add_subdirectory(third/nana)
add_subdirectory(third/gflags)

set(SCANLIB_GUI_DIR src/scangui)
file(GLOB_RECURSE SRC_SCANGUI "${SCANLIB_GUI_DIR}/*.cpp" "${SCANLIB_GUI_DIR}/*.c")
add_executable(scangui ${SRC_SCANGUI})
add_dependencies(scangui scanlib			gflags nana)
target_include_directories(scangui PUBLIC	gflags third/nana/include)
target_link_libraries(scangui scanlib		gflags nana)

# -- for Image segmentation app
find_package(OpenCV REQUIRED)
set(SCANLIB_IMSEG_DIR src/imseg)
file(GLOB_RECURSE SRC_IMSEG "${SCANLIB_IMSEG_DIR}/*.cpp" "${SCANLIB_IMSEG_DIR}/*.c")
add_executable(imseg ${SRC_IMSEG})
add_dependencies(imseg scanlib	            gflags)
target_include_directories(imseg PUBLIC		${OpenCV_INCLUDE_DIRS} gflags third/nana/include)
target_link_libraries(imseg					${OpenCV_LIBS} scanlib gflags)

# -- for test env
# source
aux_source_directory("tests" TESTSRC)

# exec
add_executable(tests ${TESTSRC})

# test depenedency
add_dependencies(tests scanlib)
target_link_libraries(tests PRIVATE scanlib)
