#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

#
PROJECT(multi_sensors_device_sample)

# Configuration
#

include_directories(
	./
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

file(GLOB SRC_FILES 
	src/*.cpp
)

add_executable(MultiSensorsDeviceSample
    ${SRC_FILES}
)

if ("${DEVICE}" MATCHES "RPi")
TARGET_LINK_LIBRARIES(MultiSensorsDeviceSample
	ccJsonParser
        ccCore
	wiringPi
	AWSIoTEmbeddedSDK
	mbedtls
	mbedcrypto
	mbedx509
	pthread
)
else ("${DEVICE}" MATCHES "RPi")
TARGET_LINK_LIBRARIES(MultiSensorsDeviceSample
	ccJsonParser
	ccCore
	AWSIoTEmbeddedSDK
	mbedtls
	mbedcrypto
	mbedx509
	pthread
)
endif ("${DEVICE}" MATCHES "RPi")
