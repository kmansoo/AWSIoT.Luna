#include <iostream>
#include <chrono>

#include "AWSIoTAgent.h"
#include "ArduinoAgent.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cout << "Usage: MultiSensorsDeviceSample [aws_connection_info.json] [certificate_file_path]" << std::endl;
		return 0;
	}
	
    AWSIoTAgent aws_iot_agent;
    ArduinoAgent arduino_agent;

    if (aws_iot_agent.init(&arduino_agent, argv[1], argv[2]))
    {
        arduino_agent.init();

        while (!aws_iot_agent.is_stop())
            std::this_thread::sleep_for(std::chrono::microseconds(100));

        arduino_agent.stop();
    }


	return 0;
}
