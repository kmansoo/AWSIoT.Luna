#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

#
PROJECT(multi_sensors_device_sample)

# Configuration
#

include_directories(
	./
	${MBED_TLS}/include 
	${AWS_IOT_EMBEDDED_SDK_LINUX}
	${LUNA_SW_PLATFORM_SDK}
)

# for debuging
add_definitions(
	-DENABLE_IOT_DEBUG
	-DENABLE_IOT_INFO
	-DENABLE_IOT_WARN
	-DENABLE_IOT_ERROR
)

add_executable(MultiSensorsDeviceSample
    src/main.cpp
	src/AWSIoTConnectionInfo.cpp
)

TARGET_LINK_LIBRARIES(MultiSensorsDeviceSample
	ccJsonParser
	AWSIoTEmbeddedSDK
	mbedtls
	mbedcrypto
	mbedx509
	pthread
)
