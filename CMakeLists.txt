cmake_minimum_required(VERSION 2.8)
project(SLAM)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

# Check C++11 or C++0x support
add_definitions(-DCOMPILEDWITHC11)
if( CMAKE_BUILD_TYPE MATCHES "Debug" )
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -g -march=native -Wno-deprecated-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -g -Wno-deprecated-declarations")
else( CMAKE_BUILD_TYPE MATCHES "Release" )
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -O3 -march=native -Wno-deprecated-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -std=c++11 -pthread -O3 -Wno-deprecated-declarations")
endif( CMAKE_BUILD_TYPE MATCHES "Debug" )


#LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

#find_package(OpenCV 3.3.1 REQUIRED)

find_package(OpenCV REQUIRED
              NO_MODULE # should be optional, tells CMake to use config mode
              PATHS /home/dm/tina3.5.1/out/mr133-perf1/staging_dir/target/usr/usr/share/OpenCV/ # look here
              NO_DEFAULT_PATH) # and don't look anywhere else

# find_package(OpenCV REQUIRED
#   NO_MODULE # should be optional, tells CMake to use config mode
#   PATHS /home/dm/tina3.5.1/out/mr133-perf1/staging_dir/target/usr/lib/ # look here
#   NO_DEFAULT_PATH) # and don't look anywhere else

#find_package(Eigen3 REQUIRED)
#find_package(Pangolin REQUIRED)

set(EIGEN3_INCLUDE_DIR /home/dm/tina3.5.1/out/mr133-perf1/staging_dir/target/usr/include/eigen3/)

# set(OpenCV_LIBS /home/dm/tina3.5.1/out/mr133-perf1/compile_dir/target/opencv-3.3.1/lib/)

# include_directories(/home/dm/tina3.5.1/out/mr133-perf1/compile_dir/target/opencv-3.3.1/lib/)

set(OTHER_LIBS /home/dm/tina3.5.1/out/mr133-perf1/staging_dir/target/usr/lib)
#set(OpenCV_LIBS ~/tina3.5.1/out/mr133-perf1/staging_dir/target/usr/usr/lib)

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/incc
${EIGEN3_INCLUDE_DIR}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED

src/System.cpp
src/Tracking.cpp
src/LocalMapping.cpp
src/LoopClosing.cpp
src/ORBextractor.cpp
src/ORBmatcher.cpp
# src/FrameDrawer.cpp
src/Converter.cpp
src/MapPoint.cpp
src/KeyFrame.cpp
src/Map.cpp
# src/MapDrawer.cpp
src/Optimizer.cpp
src/PnPsolver.cpp
src/Frame.cpp
src/KeyFrameDatabase.cpp
src/Sim3Solver.cpp
src/Initializer.cpp
# src/Viewer.cpp

srcc/app_uart.c
srcc/ConvertCP.cpp
srcc/Semaphore.cpp

)

MESSAGE("OpenCV_LIBS: " ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${EIGEN3_LIBS}
${OTHER_LIBS}
#${Pangolin_LIBRARIES}
${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so
${PROJECT_SOURCE_DIR}/Thirdparty/g2o/lib/libg2o.so
)

add_executable(mono Mono/mono.cpp)
target_link_libraries(mono ${PROJECT_NAME})

add_executable(interaction Mono/interaction.cpp)
target_link_libraries(interaction ${PROJECT_NAME})

add_executable(consumer Mono/consumer.cpp)
target_link_libraries(consumer ${PROJECT_NAME})

add_executable(producer Mono/producer.cpp)
target_link_libraries(producer ${PROJECT_NAME})

#add_executable(mono_kitti Examples/Monocular/mono_kitti.cc)
#target_link_libraries(mono_kitti ${PROJECT_NAME})

#add_executable(stereo_kitti Examples/Stereo/stereo_kitti.cpp)
#target_link_libraries(stereo_kitti ${PROJECT_NAME})

#add_executable(stereo_EuRoC Examples/Stereo/stereo_EuRoC.cpp)
#target_link_libraries(stereo_EuRoC ${PROJECT_NAME})

#add_executable(rgbd_tum Examples/RGB-D/rgbd_tum.cc)
#target_link_libraries(rgbd_tum ${PROJECT_NAME})

# Build tools
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Vocabulary)
add_executable(bin_vocabulary Vocabulary/bin_vocabulary.cpp)
target_link_libraries(bin_vocabulary ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/lib/libDBoW2.so ${OpenCV_LIBS})