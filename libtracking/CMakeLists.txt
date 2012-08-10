find_package(WiiUse REQUIRED)

include_directories(${WIIUSE_INCLUDE_DIRS})
add_library(tracking STATIC Tracking.cpp)

