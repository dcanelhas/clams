cmake_minimum_required(VERSION 2.4.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries

SET(CMAKE_BUILD_TYPE Release)

set ( CMAKE_VERBOSE_MAKEFILE true )

    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/core/clams/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/utility/eigen_extensions/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/utility/timer/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/utility/serializable/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/utility/bag_of_tricks/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/external/gzstream/include/)
include_directories(${PROJECT_SOURCE_DIR}/ros-pkg/core/stream_sequence/include/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

#set(HEADER_FILES ${PROJECT_SOURCE_DIR}/ros-pkg/utility/eigen_extensions/include/eigen_extensions.h)
set(PCL_DIR "/home/danielcanelhas/workspace/pcl/build")
find_package(PCL 1.7 REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(OpenCV REQUIRED)
include_directories(${OPENCV_INCLUDE_DIRS})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(VTK REQUIRED)
if(VTK_FOUND)
  include(${VTK_USE_FILE})
endif()

find_package(Boost 1.53 COMPONENTS program_options thread filesystem REQUIRED) #Set your own version

###Libraries built by this project
###bag_of_tricks
add_library(bag_of_tricks
  ros-pkg/utility/bag_of_tricks/src/high_res_timer.cpp
  ros-pkg/utility/bag_of_tricks/src/lockable.cpp
  )
target_link_libraries(bag_of_tricks rt boost_thread)

###timer
add_library(timer
  ros-pkg/utility/timer/src/lib/timer.cpp
  )
target_link_libraries(timer rt)

###serializable
add_library(serializable
  ros-pkg/utility/serializable/src/lib/serializable.cpp
  )

###stream_sequence

add_library(stream_sequence
  ros-pkg/core/stream_sequence/src/lib/stream_sequence_base.cpp
  ros-pkg/core/stream_sequence/src/lib/stream_sequence_pcl_wrapper.cpp
  ros-pkg/core/stream_sequence/src/lib/stream_sequence.cpp
  ros-pkg/core/stream_sequence/src/lib/frame_projector.cpp
  )
target_link_libraries(stream_sequence boost_system boost_program_options timer serializable ${PCL_COMMON_LIBRARIES})
set_target_properties(stream_sequence PROPERTIES COMPILE_FLAGS -Wno-deprecated -Wno-unused-but-set-variable -Wno-unknown-pragmas)


##external, (but still built by this project)

#eigen_extensions
add_library(eigen_extensions
ros-pkg/utility/eigen_extensions/src/random.cpp)  
  
#gzstream 
add_library(gzstream 
ros-pkg/external/gzstream/src/gzstream.cpp)
target_link_libraries(gzstream z) 


#common commands for building c++ executables and libraries
add_library(clams
  ros-pkg/core/clams/src/lib/discrete_depth_distortion_model.cpp
  ros-pkg/core/clams/src/lib/discrete_depth_distortion_model_helpers.cpp
  ros-pkg/core/clams/src/lib/slam_calibrator.cpp
  ros-pkg/core/clams/src/lib/trajectory_visualizer.cpp
  ros-pkg/core/clams/src/lib/trajectory.cpp
  )

target_link_libraries(clams 
  boost_program_options 
  boost_system 
  boost_filesystem 
  boost_thread 
  stream_sequence
  bag_of_tricks
  timer
  serializable
  stream_sequence
  eigen_extensions
  gzstream
  ${OpenCV_LIBS}
  ${PCL_LIBRARIES}
  )

add_executable(calibrate ros-pkg/core/clams/src/prog/calibrate.cpp)
target_link_libraries(
  calibrate
  clams 
  boost_filesystem  
  ${PCL_LIBRARIES}  
)

add_executable(sseq_to_freiburg ros-pkg/core/stream_sequence/src/prog/sseq_to_freiburg.cpp)
target_link_libraries(
  sseq_to_freiburg 
  stream_sequence 
  ${OpenCV_LIBS}
  ${PCL_LIBRARIES}
  gzstream
  )

add_executable(inspect ros-pkg/core/clams/src/prog/inspect.cpp)
target_link_libraries(inspect clams vtkCommon vtkFiltering vtkRendering)

add_executable(visualize_model ros-pkg/core/clams/src/prog/visualize_model.cpp)
target_link_libraries(visualize_model clams)

add_executable(convert_trajectory ros-pkg/core/clams/src/prog/convert_trajectory.cpp)
target_link_libraries(convert_trajectory clams)

add_executable(visualize_trajectory ros-pkg/core/clams/src/prog/visualize_trajectory.cpp)
target_link_libraries(visualize_trajectory clams)

add_executable(generate_map ros-pkg/core/clams/src/prog/generate_map.cpp)
target_link_libraries(generate_map clams)
