add_subdirectory(ParticleFilter)

rosbuild_add_library(tracking HumanTracker.cpp ParticleFilter/Condensation.cpp ParticleFilter/ParticleFilter.cpp ParticleFilter/StateData.cpp)
