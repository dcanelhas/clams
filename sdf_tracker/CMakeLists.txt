cmake_minimum_required(VERSION 2.4.6)
SET(PROJECT_NAME sdf_tracker)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries

SET(CMAKE_BUILD_TYPE Release)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# check for SSE flags
include(${PROJECT_SOURCE_DIR}/cmake/Find_SSE.cmake)
CHECK_FOR_SSE()
message(STATUS "SSE instructions supported and enabled: ${SSE_FLAGS}")

#check for OpenMP
find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  if(MSVC90 OR MSVC10)
    if(MSVC90)
      set(OPENMP_DLL VCOMP90)
    elseif(MSVC10)
      set(OPENMP_DLL VCOMP100)
    endif(MSVC10)
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DELAYLOAD:${OPENMP_DLL}D.dll")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DELAYLOAD:${OPENMP_DLL}.dll")
  endif(MSVC90 OR MSVC10)
else(OPENMP_FOUND)
  message (STATUS "OpenMP not found")
endif(OPENMP_FOUND)

if (MSVC)
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /O2 ${SSE_FLAGS}")
else (MSVC)
  set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS} ${SSE_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g ${SSE_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS} -O3 ${SSE_FLAGS}")
endif (MSVC)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
#find_package(OpenNI2 REQUIRED)
find_package(VTK REQUIRED)
find_package(Boost 1.49 COMPONENTS system filesystem REQUIRED) #Set your own version

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${VTK_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${OPENCV_INCLUDE_DIRS})
#include_directories(${OPENNI2_INCLUDE_DIR}) 
include_directories(./)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${OPENCV_LIBRARY_DIRS})
#link_directories(${OPENNI2_LIBRARIES})
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -std=c++0x")
###############################################################################
add_library(${PROJECT_NAME} src/sdf_tracker.cpp include/sdf_tracker.h)

target_link_libraries(${PROJECT_NAME} 
  ${OpenCV_LIBS} 
  ${VTK_LIBRARIES}
  ${Boost_LIBRARIES}
)

###############################################################################
add_executable(
  tracker_clams
  src/tracker_clams.cpp)

target_link_libraries(tracker_clams 
  ${PROJECT_NAME}
  ${OpenCV_LIBS} 
  boost_filesystem
  boost_system
)