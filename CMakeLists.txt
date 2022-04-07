#acg opengl kernel from openflipper

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ACGViewer)

cmake_policy(SET CMP0100 NEW)

# add our macro directory to cmake search path
#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(Qt5 COMPONENTS Core)

# Enable automoc, autouic and auto rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(Qt5Core_FOUND)
  set(QT5_FOUND TRUE)
  set(QT5_BIN_DIR ${_qt5_install_prefix}/../../bin)
endif(Qt5Core_FOUND)

option(USE_THIRDPARTY_SDK "use thirdparty sdks." ON)

if(USE_THIRDPARTY_SDK)
    set(THIRDPARTY_SDK_DIR "D:/thirdparty" CACHE PATH "Directory of thirdparty sdks.")

    set(OpenMesh_DIR "${THIRDPARTY_SDK_DIR}/openmesh/share/OpenMesh/cmake" CACHE PATH "Directory of openmesh")
    set(GLEW_DIR "${THIRDPARTY_SDK_DIR}/glew/lib/cmake/glew" CACHE PATH "Directory of glew")
    set(GTest_DIR "${THIRDPARTY_SDK_DIR}/gtest/lib/cmake/GTest" CACHE PATH "Directory of google test.")
    set(VLD_DIR "${THIRDPARTY_SDK_DIR}/vld" CACHE PATH "Directory of vld.")
endif()

# check for OpenGL, GLEW as our required dependencies
find_package (OpenGL)

if (NOT OPENGL_FOUND)
  message (FATAL_ERROR "OpengGL not found!")
endif ()

find_package (OpenMesh REQUIRED)

find_package (GLEW REQUIRED)

option(BUILD_TEST "Build test." OFF)
if(BUILD_TEST)
  find_package(GTest REQUIRED)
  set(GTEST_BIN_DIR ${GTEST_INCLUDE_DIR}/../bin)
endif(BUILD_TEST)

option(BUILD_VLD "Visual leak detect." OFF)
if(BUILD_VLD)
  set(VLD_INCLUDE_DIR "${VLD_DIR}/include" CACHE PATH "Directory of vld include.")
  set(VLD_LIBRARY_DIR "${VLD_DIR}/lib" CACHE PATH "Directory of vld library.")
  set(VLD_RUNTIME_DIR "${VLD_DIR}/bin" CACHE PATH "Directory of vld runtime.")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "_d")

add_subdirectory(src)
