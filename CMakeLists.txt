cmake_minimum_required(VERSION 3.26)
project(BluetoothGlucose)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)
include(FetchContent)
find_package(Git REQUIRED)
find_package(simpleble REQUIRED)

FetchContent_Declare(
        rspcorelib
        GIT_REPOSITORY "git@github.com:rsps/rsp-core-library.git"
        GIT_TAG "origin/gfx_acc"
)
FetchContent_MakeAvailable(rspcorelib)

MESSAGE("SimpleBluez: ${simplebluez_inc}")

include_directories(
        ${CMAKE_BINARY_DIR}/include/
        ${rspcorelib_SOURCE_DIR}/include
        ${simplebluez_inc}
)

add_subdirectory(src/ble-dump)
