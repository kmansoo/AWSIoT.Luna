/*
* ArduinoAgent.h
*
*  Created on: 2016. 9. 6.
*      Author: Mansoo Kim
*/

#ifndef __ARDUINO_AGENT_AGENT_H__
#define __ARDUINO_AGENT_AGENT_H__

#include <thread>
#include <string>

class ArduinoAgent {
public:
    ArduinoAgent();
    ~ArduinoAgent();

public:
    bool init();
    void stop();

    float get_temperature() { return temperature_; }
    float get_humidity() { return humidity_; }
    float get_distance() { return distance_; }

private:
    std::thread thread_;
    std::string device_name_;

    int handle_;
    bool stop_;

    // read signal
    int json_data_buf_index_;
    char json_data_buf_[2048];

    // current sensor data
    float temperature_;
    float humidity_;
    float distance_;
};

#endif
