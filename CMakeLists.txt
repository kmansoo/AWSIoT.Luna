# CMake Version 
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# Project Name
PROJECT(aws_iot_with_luna)

#
# Configuration
#
# SET(CMAKE_VERBOSE_MAKEFILE ON)
#

# If you want to build with Razpberry toolchain for RPi 2 or 3, you must use following option with cmake.
# cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.rpi.arm.cmake

# build mode
# cmake -DCMAKE_BUILD_TYPE=Debug
#
if ("${build}" MATCHES "debug")
 SET (CMAKE_BUILD_TYPE "debug")
else ("${build}" MATCHES "debug")
 SET (CMAKE_BUILD_TYPE "release")
endif ("${build}" MATCHES "debug")

#
# Add Compiler Option and Macro
#
ADD_DEFINITIONS(-DCALIB_CFG_BUILD_MODE="${CMAKE_BUILD_TYPE}")

# Set Build Directories
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(LUNA_SW_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/dependency/Luna)
SET(LUNA_SW_PLATFORM_SDK 
	${LUNA_SW_PLATFORM}/src/Library
	${LUNA_SW_PLATFORM}/src/FossLibrary
	${LUNA_SW_PLATFORM}/src/ThirdPartyLibrary
	${LUNA_SW_PLATFORM}/src/ProductLibrary
)

SET(MBED_TLS ${CMAKE_CURRENT_SOURCE_DIR}/dependency/mbedTLS)
SET(AWS_IOT_EMBEDDED_SDK ${CMAKE_CURRENT_SOURCE_DIR}/dependency/AWSIoTEmbeddedSDK)

SET(AWS_IOT_EMBEDDED_SDK_LINUX 
	${AWS_IOT_EMBEDDED_SDK}/include
	${AWS_IOT_EMBEDDED_SDK}/platform/linux/common
	${AWS_IOT_EMBEDDED_SDK}/platform/linux/mbedtls
	${AWS_IOT_EMBEDDED_SDK}/platform/linux/pthread		
)

#
# Check OS and Processor Information 
#
SET(CMAKE_CXX_FLAGS "-std=c++11")
ADD_DEFINITIONS(-Wall -DCALIB_CFG_OS="${CMAKE_SYSTEM_NAME}" -DCALIB_CFG_CPU="${CMAKE_SYSTEM_PROCESSOR}")

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#
# Make Libraries
#
ADD_SUBDIRECTORY(dependency/Luna)
ADD_SUBDIRECTORY(dependency/mbedTLS)
ADD_SUBDIRECTORY(dependency/AWSIoTEmbeddedSDK)

#
# Make Test Applications
#
ADD_SUBDIRECTORY(src/MultiSensorsDeviceSample)

