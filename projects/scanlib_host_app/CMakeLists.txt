cmake_minimum_required(VERSION 2.8)

project(scanlib)
set(CMAKE_CXX_STANDARD 17)

# Core library. Includes protocol.
include_directories(src)

# Core functionalities
set(SCANLIB_DIR src/scanlib)
set(SCANLIB_ARCH_DIR ${SCANLIB_DIR}/arch)
file(GLOB_RECURSE SRC_SCANLIB "${SCANLIB_DIR}/*.cpp" "${SCANLIB_DIR}/*.c")
file(GLOB_RECURSE HEADER_SCANLIB "${SCANLIB_DIR}/*.h" "${SCANLIB_DIR}/*.hpp" "${SCANLIB_DIR}/*.hxx")
list(REMOVE_ITEM SRC_SCANLIB "${SCANLIB_DIR}/arch/**")
install(FILES ${HEADER_SCANLIB} DESTINATION scanlib/include/scanlib)

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

INSTALL ( TARGETS scanlib
        RUNTIME DESTINATION scanlib/bin
        LIBRARY DESTINATION scanlib/lib
        ARCHIVE DESTINATION scanlib/lib
        )

# -- for GUI app
add_subdirectory(third/nana)
add_subdirectory(third/gflags)

set(SCANLIB_GUI_DIR src/scangui)
file(GLOB_RECURSE SRC_SCANGUI "${SCANLIB_GUI_DIR}/*.cpp" "${SCANLIB_GUI_DIR}/*.c")
add_executable(scangui ${SRC_SCANGUI})
add_dependencies(scangui scanlib			gflags nana)
target_include_directories(scangui PUBLIC	gflags third/nana/include)
target_link_libraries(scangui scanlib		gflags nana)

INSTALL ( TARGETS scangui
        RUNTIME DESTINATION scangui/bin
        LIBRARY DESTINATION scangui/lib
        ARCHIVE DESTINATION scangui/lib
        )

# -- for Image segmentation app 
if(1)
    # CUDA settings
    find_package(CUDA REQUIRED)
    set(CUDA_COMPUTE_CAPABILITY "61")
    set(CUDA_GENERATE_CODE "arch=compute_${CUDA_COMPUTE_CAPABILITY},code=sm_${CUDA_COMPUTE_CAPABILITY}" CACHE STRING "Which GPU architectures to generate code for (each arch/code pair will be passed as --generate-code option to nvcc, separate multiple pairs by ;)")
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -arch compute_${CUDA_COMPUTE_CAPABILITY})

    # SLIC algorithm dir
    set(CUDA_SLIC_DIR third/SLIC_CUDA)
    set(CUDA_SLIC_SRC ${CUDA_SLIC_DIR}/src/SlicCudaDevice.cu
                      ${CUDA_SLIC_DIR}/src/SlicCudaHost.cu)
    set(CUDA_SLIC_INCLUDE_DIR ${CUDA_SLIC_DIR}/include)

    # OpenCV Package
    find_package(OpenCV REQUIRED)

    # Configure build
    set(SCANLIB_IMSEG_DIR src/imseg)
    file(GLOB_RECURSE SRC_IMSEG "${SCANLIB_IMSEG_DIR}/*.cpp" "${SCANLIB_IMSEG_DIR}/*.c")
    cuda_add_executable(imseg ${SRC_IMSEG} ${CUDA_SLIC_SRC})
    add_dependencies(imseg scanlib	            gflags)
    target_include_directories(imseg PUBLIC		${OpenCV_INCLUDE_DIRS} gflags third/nana/include ${CUDA_SLIC_INCLUDE_DIR})
    target_link_libraries(imseg					${OpenCV_LIBS} scanlib gflags)

endif()
# -- for test env
# source
aux_source_directory("tests" TESTSRC)

# exec
add_executable(tests ${TESTSRC})

# test depenedency
add_dependencies(tests scanlib)
target_link_libraries(tests PRIVATE scanlib)
