set(CMAKE_CXX_FLAGS "${CMAKE_CSS_FLAGS} -std=gnu++0x")

file(GLOB_RECURSE ANALYSIS_PACK package_analysis/*.cpp)
file(GLOB_RECURSE BGS_PACK package_bgs/*.cpp)

file(GLOB SRC *.cpp)
file(GLOB DEMO Demo.cpp)

list(REMOVE_ITEM SRC ${DEMO})

rosbuild_add_library(bgs ${ANALYSIS_PACK} ${BGS_PACK})

rosbuild_add_executable(bgs_demo ${SRC})

target_link_libraries(bgs_demo bgs)
