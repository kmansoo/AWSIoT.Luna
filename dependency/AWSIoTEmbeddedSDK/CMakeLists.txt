# CMake Version 
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# Project Name
PROJECT(aws_iot_sdk)

#
# Configuration

include_directories( 
	./
	./include
	./platform/linux/common
	./platform/linux/mbedtls
	./platform/linux/pthread
	./external_libs/jsmn
	${MBED_TLS}/include
)

file(GLOB SRC_FILES 
	src/*.*
	platform/linux/common/*.c
	platform/linux/mbedtls/*.c
	platform/linux/pthread/*.c
	external_libs/jsmn/*.c
)

add_library (AWSIoTEmbeddedSDK
	${SRC_FILES}
)
