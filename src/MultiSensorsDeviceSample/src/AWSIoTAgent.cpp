/*
 * AWSIoTAgent.cpp
 *
 *  Created on: 2016. 9. 6.
 *      Author: Mansoo Kim
 */

#include "ccCore/ccString.h"

#include <iostream>

#ifndef WIN32
#   include <unistd.h>
#else
#   define  PATH_MAX    512
#   define  __PRETTY_FUNCTION__ __FUNCTION__
#endif

#include "AWSIoTAgent.h"

//  constructor
AWSIoTAgent::AWSIoTAgent() : arduino_agent_(nullptr), stop_(false) {

}

AWSIoTAgent::~AWSIoTAgent() {
    stop();
}

bool AWSIoTAgent::init(ArduinoAgent* agent, const std::string& aws_config_fileanme, const std::string& certificate_files_path) {
    arduino_agent_ = agent;

    char rootCA[PATH_MAX + 1];
    char clientCRT[PATH_MAX + 1];
    char clientKey[PATH_MAX + 1];
    char cPayload[100];

    int32_t i = 0;

    IoT_Error_t rc = FAILURE;

    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    if (conn_info_.load_file(aws_config_fileanme) == false) {
        std::cout << "I couldn't parse '" << aws_config_fileanme << "' for IoT connection info.'" << std::endl;
        return false;
    }

    IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    snprintf(rootCA, PATH_MAX + 1, "%s/%s", certificate_files_path.c_str(), conn_info_.get_root_ca_filename().c_str());
    snprintf(clientCRT, PATH_MAX + 1, "%s/%s", certificate_files_path.c_str(), conn_info_.get_certificate_filename().c_str());
    snprintf(clientKey, PATH_MAX + 1, "%s/%s", certificate_files_path.c_str(), conn_info_.get_private_key_filename().c_str());

    IOT_DEBUG("rootCA %s", rootCA);
    IOT_DEBUG("clientCRT %s", clientCRT);
    IOT_DEBUG("clientKey %s", clientKey);

    mqttInitParams.enableAutoReconnect = true; // We enable this later below
    mqttInitParams.pHostURL = (char*)conn_info_.get_mqtt_host().c_str();
    mqttInitParams.port = conn_info_.get_mqtt_port();
    mqttInitParams.pRootCALocation = rootCA;
    mqttInitParams.pDeviceCertLocation = clientCRT;
    mqttInitParams.pDevicePrivateKeyLocation = clientKey;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = this;

    rc = aws_iot_mqtt_init(&client_, &mqttInitParams);
    if (SUCCESS != rc) {
        IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);

        stop_ = false;
        return false;
    }

    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = (char *)conn_info_.get_mqtt_client_id().c_str();
    connectParams.clientIDLen = conn_info_.get_mqtt_client_id().length();
    connectParams.isWillMsgPresent = false;

    IOT_INFO("Connecting...");

    rc = aws_iot_mqtt_connect(&client_, &connectParams);

    if (SUCCESS != rc) {
        IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);

        stop_ = false;

        return false;
    }

    /*
    * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
    *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
    *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
    */
    rc = aws_iot_mqtt_autoreconnect_set_status(&client_, true);
    if (SUCCESS != rc) {
        IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);

        stop_ = false;

        return false;
    }

    IOT_INFO("Subscribing...");

    rc = aws_iot_mqtt_subscribe(&client_, TOPIC_MANSOO_RETRIEVE_TEMPERATURE, strlen(TOPIC_MANSOO_RETRIEVE_TEMPERATURE), QOS0, iot_subscribe_callback_handler, this);
    rc = aws_iot_mqtt_subscribe(&client_, TOPIC_MANSOO_RETRIEVE_DISTANCE, strlen(TOPIC_MANSOO_RETRIEVE_DISTANCE), QOS1, iot_subscribe_callback_handler, this);

    if (SUCCESS != rc) {
        IOT_ERROR("Error subscribing : %d ", rc);

        stop_ = false;

        return false;
    }

    sprintf(cPayload, "%s : %d ", "hello from SDK", i);

    paramsQOS0_.qos = QOS0;
    paramsQOS0_.payload = (void *)payload_buffer_.c_str();
    paramsQOS0_.isRetained = 0;

    paramsQOS1_.qos = QOS1;
    paramsQOS1_.payload = (void *)payload_buffer_.c_str();
    paramsQOS1_.isRetained = 0;

    thread_ = std::thread([&] {
        IoT_Error_t status = SUCCESS;

        while (stop_ == false) {
            status = aws_iot_mqtt_yield(&client_, 100);
        }

        if (SUCCESS != rc) {
            IOT_ERROR("An error occurred in the loop.\n");
        }
        else {
            IOT_INFO("Publish done\n");
        }
    });

    return true;
}

void AWSIoTAgent::stop() {
    if (stop_) {
        stop_ = false;

        thread_.join();
    }
}

void AWSIoTAgent::iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
    IoT_Publish_Message_Params *params, void *pData) {

    IOT_UNUSED(pData);
    IOT_UNUSED(pClient);

    std::string topic_name;
    topic_name.assign(topicName, topicNameLen);

    AWSIoTAgent* agent = (AWSIoTAgent*)pData;

    if (topic_name == TOPIC_MANSOO_RETRIEVE_TEMPERATURE) {

        Luna::ccString::format(
            agent->payload_buffer_,
            "{\"device\": {\"type\": \"temperature_sensor\", \"temperature\" : %4.1f, \"humidity\" : %4.1f}}\n",
            agent->arduino_agent_->get_temperature(),
            agent->arduino_agent_->get_humidity());

        std::cout << "Payload: " << agent->payload_buffer_ << std::endl;

        agent->paramsQOS0_.payload = (void*)agent->payload_buffer_.c_str();
        agent->paramsQOS0_.payloadLen = agent->payload_buffer_.length();

        IoT_Error_t rc = aws_iot_mqtt_publish(pClient, TOPIC_MANSOO_TEMPERATURE_SENSOR_STATUS, strlen(TOPIC_MANSOO_TEMPERATURE_SENSOR_STATUS),  &(agent->paramsQOS0_));
    }

    if (topic_name == TOPIC_MANSOO_RETRIEVE_DISTANCE) {
        Luna::ccString::format(
            agent->payload_buffer_,
            "{\"device\": {\"type\": \"ultrasonic_sensor\", \"distance\" : %7.3f}}\n",
            agent->arduino_agent_->get_distance());

        //  std::cout << "Payload: " << agent->payload_buffer_ << std::endl;

        agent->paramsQOS1_.payload = (void*)agent->payload_buffer_.c_str();
        agent->paramsQOS1_.payloadLen = agent->payload_buffer_.length();

        IoT_Error_t rc = aws_iot_mqtt_publish(pClient, TOPIC_MANSOO_ULTRASONIC_SENSOR_STATUS, strlen(TOPIC_MANSOO_ULTRASONIC_SENSOR_STATUS), &(agent->paramsQOS1_));
    }

    IOT_INFO("Subscribe callback");


    IOT_INFO("%.*s\t%.*s", topicNameLen, topicName, (int)params->payloadLen, params->payload);
}

void AWSIoTAgent::disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
    IOT_WARN("MQTT Disconnect");

    IoT_Error_t rc = FAILURE;

    if (NULL == pClient) {
        return;
    }

    IOT_UNUSED(data);

    if (aws_iot_is_autoreconnect_enabled(pClient)) {
        IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else {
        IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");

        rc = aws_iot_mqtt_attempt_reconnect(pClient);

        if (NETWORK_RECONNECTED == rc) {
            IOT_WARN("Manual Reconnect Successful");
        }
        else {
            IOT_WARN("Manual Reconnect Failed - %d", rc);

            ((AWSIoTAgent*)data)->stop();
        }
    }
}
