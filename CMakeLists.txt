cmake_minimum_required (VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project (MissileLauncher)

add_subdirectory(libmissile)
add_subdirectory(libtracking)
add_subdirectory(missilelauncher)
