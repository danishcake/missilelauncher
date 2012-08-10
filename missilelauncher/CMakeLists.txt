include_directories (${MissileLauncher_SOURCE_DIR}/libmissile)
include_directories (${MissileLauncher_SOURCE_DIR}/libtracking)

link_directories (${HELLO_BINARY_DIR}/libmissile)
link_directories (${HELLO_BINARY_DIR}/libtracking)
find_package(WiiUse REQUIRED)
find_package(Libusb1 REQUIRED)
message (STATUS "libusb: " ${LIBUSB1_LIBRARIES})
message (STATUS "Wiiuselib: " ${WIIUSE_LIBRARIES})
link_directories (${LIBUSB1_LIBRARIES})
link_directories (${WIIUSE_LIBRARIES})





add_executable(missilelauncher Main.cpp)
target_link_libraries(missilelauncher missile tracking usb-1.0 wiiuse)





