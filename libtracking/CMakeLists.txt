find_package(WiiUse REQUIRED)
find_package(Boost  REQUIRED)

include_directories(${WIIUSE_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

add_library(tracking STATIC Tracking.cpp)

