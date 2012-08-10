find_package(Libusb1 REQUIRED)
find_package(Boost  REQUIRED)
find_package(PythonLibs  REQUIRED)

include_directories(${LIBUSB1_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})


add_library(missile STATIC Init.cpp Actions.cpp)
add_library(pymissile SHARED Init.cpp Actions.cpp Python.cpp)

