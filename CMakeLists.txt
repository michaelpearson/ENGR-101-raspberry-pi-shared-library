cmake_minimum_required(VERSION 2.8)
project(engr101library)


SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER /home/michael/Desktop/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /home/michael/Desktop/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-g++)
SET(CMAKE_FIND_ROOT_PATH /home/michael/Desktop/rpi/fs)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_BUILD_TYPE Release)


include_directories(lib/camera/include)
include_directories(lib/camera/include/interface/vcos/pthreads)
include_directories(lib/camera/include/interface/vmcs_host/linux)
include_directories(lib/camera)
include_directories(lib/PIGPIO)


link_directories(/home/michael/Desktop/rpi/ENGR-101-raspberry-pi-shared-library/lib/camera/lib/)

add_library(engr101library SHARED main.cpp
        lib/camera/camera.cpp
        lib/camera/cameracontrol.cpp
        lib/PIGPIO/pigpio.c
        lib/PIGPIO/command.c)

target_link_libraries(engr101library
        pthread
        rt
        libmmal_core.so
        libmmal_util.so
        libmmal_vc_client.so
        libvcos.so
        libbcm_host.so)