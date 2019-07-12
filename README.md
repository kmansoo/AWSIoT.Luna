# AWSIoT.Luna
This project aims to show how to use AWS IoT Embedded C SDK with Luna SW Platform.

# How to build
## Win32
AWSIoT.Luna has a solution file for Visual Studio 2015, so you can build very easy.
You can find and use a solution file for visual studio 2005 like './msvs/2005/AWSIoT.Luna.sin'.

## macOS(OS X) and Linux(i386)
```bash
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make -j4
```

## Raspberry Pi2 or Pi3
```bash
git submodule init
git submodule update
mkdir build
cd build
cmake .. -DDEVICE=RPi
make -j4
```

## Arduino
You can find a project file in ./src/Arduino/multi_sensors_device_01 using Arduino IDE.

# How to test
```bash
./bin/MultiSensorsDeviceSample [aws_iot_config.json] [certificates_file_path]
```

# Reference Sources
1. SDK for connecting to AWS IoT from a device using embedded C: https://github.com/aws/aws-iot-device-sdk-embedded-C
