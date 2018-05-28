cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#rosbuild_add_library(cv_utils src/CvUtils.cpp)

#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)

#rosbuild_add_executable(example examples/example.cpp)
rosbuild_add_executable(ac_vision src/ac_vision.cpp)

#target_link_libraries(example ${PROJECT_NAME})
target_link_libraries(ac_vision misc_utils cv_utils tracking models)

add_definitions( -DDEBUG )

#add_subdirectory(src/bgslibrary)
add_subdirectory(src/models)
add_subdirectory(src/tracking)
add_subdirectory(src/utils)

# TESTING
rosbuild_add_executable(canny test/canny.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(condens_tracking test/condens_tracking.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(contours test/contours.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(check_ml_results test/ml/check_ml_results.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(ml_accuracy test/ml/ml_accuracy.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(compare_ground_truth test/ground_truth/compare_ground_truth.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(average_results test/ground_truth/average_results.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(create_ground_truth test/ground_truth/create_ground_truth.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(display_results test/ground_truth/display_results.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(extract_background test/extract_background.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(extract_ground_truths test/ground_truth/extract_ground_truths.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(morphology test/morphology.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(resize_images test/resize_images.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(segmentation test/segmentation.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(squareise_groundtruths test/ground_truth/squareise_groundtruths.cpp src/utils/opencv/CvUtils.cpp test/ground_truth/utils/GtUtils.cpp)
rosbuild_add_executable(training_ml test/ml/training_ml.cpp src/utils/opencv/CvUtils.cpp)
rosbuild_add_executable(testing_ml test/ml/testing_ml.cpp src/utils/opencv/CvUtils.cpp)

target_link_libraries(canny misc_utils models)
target_link_libraries(condens_tracking misc_utils models)
target_link_libraries(contours misc_utils models)
target_link_libraries(check_ml_results misc_utils models)
target_link_libraries(ml_accuracy misc_utils models)
target_link_libraries(compare_ground_truth misc_utils models)
target_link_libraries(average_results misc_utils models)
target_link_libraries(create_ground_truth misc_utils models)
target_link_libraries(display_results misc_utils models)
target_link_libraries(extract_background misc_utils models)
target_link_libraries(extract_ground_truths misc_utils models)
target_link_libraries(morphology misc_utils models)
target_link_libraries(resize_images misc_utils models)
target_link_libraries(segmentation misc_utils models)
target_link_libraries(squareise_groundtruths misc_utils models)
target_link_libraries(training_ml misc_utils models)
target_link_libraries(testing_ml misc_utils models)

add_subdirectory(test/ground_truth)
add_subdirectory(test/ml)
