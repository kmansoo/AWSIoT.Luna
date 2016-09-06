# CMake Version 
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# Project Name
PROJECT(aws_iot_sdk)

#
# Configuration

include_directories( 
	./
	./include
	./platform/common_timer
	./platform/linux/mbedtls
	./platform/linux/pthread
	./external_libs/jsmn
	${MBED_TLS}/include
)

file(GLOB SRC_FILES 
	src/*.*
	platform/common_timer/*.cpp
	platform/linux/mbedtls/*.c
	platform/linux/pthread/*.c
	external_libs/jsmn/*.c
)

add_library (AWSIoTEmbeddedSDK
	${SRC_FILES}
)
