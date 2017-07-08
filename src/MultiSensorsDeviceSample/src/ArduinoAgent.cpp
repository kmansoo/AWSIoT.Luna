/*
* ArduinoAgent.cpp
*
*  Created on: 2016. 9. 6.
*      Author: Mansoo Kim
*/

#include <iostream>

#include "json/reader.h"

#include "ArduinoAgent.h"

#ifdef LUNA_PLATFORM_RPI
#   include <stdint.h> //uint8_t definitions
#   include <errno.h> //error output
#   include <wiringPi.h>
#   include <wiringSerial.h>
#else

int serialOpen(const char* device_name, int baud) {
    return 1;
}

int wiringPiSetup() {
    return 1;
}

bool serialDataAvail(int fd) {
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    return true;
}

char serialGetchar(int fd) {
    static int s_index = 0;
    static const std::string s_dummy_raw_data =
        "{\"device\": {\"type\": \"ultrasonic_sensor\", \"distance\" : 3073.702   }}\n" \
        "{\"device\": {\"type\": \"temperature_sensor\", \"temperature\" : 23.0, \"humidity\" : 17.0   }}\n" \
        "{\"device\": {\"type\": \"ultrasonic_sensor\", \"distance\" : 3.757   }}\n" \
        "{\"device\": {\"type\": \"ultrasonic_sensor\", \"distance\" : 22.321   }}\n" \
        "{\"device\": {\"type\": \"temperature_sensor\", \"temperature\" : 24.0, \"humidity\" : 40.0   }}\n";

    char ch = s_dummy_raw_data[s_index++];

    if (s_index >= s_dummy_raw_data.length())
        s_index = 0;

    return ch;
}

#endif

ArduinoAgent::ArduinoAgent() : stop_(false), json_data_buf_index_(0) {
    device_name_ = "/dev/ttyACM0";  // ARDUINO_UNO      "/dev/ttyACM0"
                                    // FTDI_PROGRAMMER  "/dev/ttyUSB0"
                                    // HARDWARE_UART    "/dev/ttyAMA0"

    if (init() == true) {
        thread_ = std::thread([&] {
            while (stop_ == false) {
                if (!serialDataAvail(handle_))
                    continue;

                char newChar = serialGetchar(handle_);

                json_data_buf_[json_data_buf_index_++] = newChar;

                if (newChar == '\n') {
                    json_data_buf_[json_data_buf_index_] = '\0';
                    json_data_buf_index_ = 0;

                    Json::Reader    reader;
                    Json::Value     root_value;

                    if (!reader.parse(json_data_buf_, root_value))
                        continue;

                    if (root_value.isObject() == false)
                        continue;

                    if (root_value["device"].isNull())
                        continue;

                    if (root_value["device"]["type"].asString() == "temperature_sensor") {
                        temperature_ = root_value["device"]["temperature"].asFloat();
                        humidity_ = root_value["device"]["humidity"].asFloat();

                        //  std::cout << "temperature: " << temperature_ << ", humidity: " << humidity_ << std::endl;
                        continue;
                    }

                    if (root_value["device"]["type"].asString() == "ultrasonic_sensor") {
                        distance_ = root_value["device"]["distance"].asFloat();
                        
                        //  std::cout << "distance: " << distance_ << std::endl;
                        continue;
                    }

                    //  std::cout << "Device.type: " << root_value["device"]["type"].asString() << std::endl;
                }
            }
        });
    }
}

ArduinoAgent::~ArduinoAgent() {
    stop();
}

bool ArduinoAgent::init() {
    unsigned long baud = 9600;

    // get filedescriptor
    if ((handle_ = serialOpen(device_name_.c_str(), baud)) < 0) {
        // fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return false;
    }

    // setup GPIO in wiringPi mode
    if (wiringPiSetup() == -1) {
        // fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        return false;
    }

    return true;
}

void ArduinoAgent::stop() {
    if (stop_) {
        stop_ = false;

        thread_.join();
    }
}
