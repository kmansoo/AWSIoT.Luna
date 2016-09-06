/*
 * AWSIoTAgent.h
 *
 *  Created on: 2016. 9. 6.
 *      Author: Mansoo Kim
 */

#ifndef __AWS_IOT_AGENT_H__
#define __AWS_IOT_AGENT_H__

#include <string>
#include <thread>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "AWSIoTConnectionInfo.h"
#include "ArduinoAgent.h"

//  defines topic
#define TOPIC_MANSOO_RETRIEVE_TEMPERATURE           "topic.mansoo.retrieve.temperature"
#define TOPIC_MANSOO_RETRIEVE_REALTIME_TEMPERATURE  "topic.mansoo.retrieve.realtime.temperature"

#define TOPIC_MANSOO_RETRIEVE_DISTANCE              "topic.mansoo.retrieve.distance"
#define TOPIC_MANSOO_RETRIEVE_REALTIME_DISTANCE     "topic.mansoo.retrieve.realtime.temperature"

#define TOPIC_MANSOO_TEMPERATURE_SENSOR_STATUS      "topic.mansoo.temperature.sensor_status" // {"device": { "type": "temperature_sensor", "temperature": 25.0, "humidity": 17.0 }}
#define TOPIC_MANSOO_ULTRASONIC_SENSOR_STATUS       "topic.mansoo.ultrasonic.sensor_status"  // {"device": { "type": "ultrasonic_sensor", "distance":  3.757}}

class AWSIoTAgent {
public:
    AWSIoTAgent();
    ~AWSIoTAgent();

public:
    bool init(ArduinoAgent* agent, const std::string& aws_config_fileanme, const std::string& certificate_files_path);
    void stop();
    bool is_stop() { return stop_; }

protected:
    //  local functions
    static void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData);
    static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data);

public:
    ArduinoAgent* arduino_agent_;

    std::string payload_buffer_;

    std::thread thread_;
    bool stop_;

    AWS_IoT_Client client_;
    AWSIoTConnectionInfo conn_info_;

    IoT_Publish_Message_Params paramsQOS0_;
    IoT_Publish_Message_Params paramsQOS1_;
};

#endif
