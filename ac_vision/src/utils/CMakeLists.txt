add_subdirectory(opencv)
add_subdirectory(ros)

rosbuild_add_library(misc_utils ApplicationSession.cpp ApplicationSettings.cpp FileUtils.cpp StringUtils.cpp)
