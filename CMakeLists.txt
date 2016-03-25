cmake_minimum_required(VERSION 2.8.1)
project(engr101)

SET(CC_ROOT /home/michael/Desktop/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-)

SET(CMAKE_C_COMPILER ${CC_ROOT}gcc)
SET(CMAKE_CXX_COMPILER ${CC_ROOT}g++)

include_directories(lib/camera/include)
include_directories(lib/camera/include/interface/vcos/pthreads)
include_directories(lib/camera/include/interface/vmcs_host/linux)
include_directories(lib/camera)
include_directories(lib/wiringPi/wiringPi)

link_directories(lib/camera/lib/)

add_library(engr101 SHARED
        main.cpp
        lib/camera/camera.cpp
        lib/camera/cameracontrol.cpp)

target_link_libraries(engr101
        pthread
        rt
        libmmal_core.so
        libmmal_util.so
        libmmal_vc_client.so
        libvcos.so
        libbcm_host.so)
