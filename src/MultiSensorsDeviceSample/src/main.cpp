/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file subscribe_publish_cpp_sample.cpp
 * @brief simple MQTT publish and subscribe on the same topic in C++
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef WIN32
#   include <unistd.h>
#else
#   define  PATH_MAX    512
#   define  __PRETTY_FUNCTION__ "client:"
#endif

#include <limits.h>
#include <string.h>

#include <string>
#include <thread>
#include <iostream>

// wiring Pi
#include "ccJsonParser/json/reader.h"

#ifdef LUNA_PLATFORM_RPI

#   include <stdint.h> //uint8_t definitions
#   include <errno.h> //error output
#   include <wiringPi.h>
#   include <wiringSerial.h>

// Find Serial device on Raspberry with ~ls /dev/tty*
// ARDUINO_UNO "/dev/ttyACM0"
// FTDI_PROGRAMMER "/dev/ttyUSB0"
// HARDWARE_UART "/dev/ttyAMA0"
char device[]= "/dev/ttyACM0";
// filedescriptor
int fd;
unsigned long baud = 9600;

//prototypes
void loop(void);
void setup(void);

void setup(){

  printf("%s \n", "Raspberry Startup!");
  fflush(stdout);

  //get filedescriptor
  if ((fd = serialOpen (device, baud)) < 0){
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    exit(1); //error
  }

  //setup GPIO in wiringPi mode
  if (wiringPiSetup () == -1){
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    exit(1); //error
  }

}

void loop(){
  // read signal
  static int json_data_buf_index = 0;
  static char json_data_buf[2048];

  if(serialDataAvail (fd)){
    char newChar = serialGetchar (fd);

    json_data_buf[json_data_buf_index++] = newChar;

    if (newChar == '\n') {
       json_data_buf[json_data_buf_index] = '\0';
       json_data_buf_index = 0;

       Json::Reader    reader;
       Json::Value     root_value;

       if (!reader.parse(json_data_buf, root_value)) {
          return;
       }

       if (root_value.isObject() == false)  {
          return;
       }

       if (root_value["device"].isNull()) {
          return;
       }

       std::cout << "Device.type: " << root_value["device"]["type"].asString() << std::endl;
     }
  }

}
#endif // LUNA_PLATFORM_RPI

////////////////////

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "AWSIoTConnectionInfo.h"

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 100;

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	IOT_INFO("Subscribe callback");

	std::string* caller_name = (std::string*)pData;
	IOT_INFO("Caller: %s", caller_name->c_str());

	IOT_INFO("%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, params->payload);
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}

int main(int argc, char *argv[]) {
	bool infinitePublishFlag = true;

	std::string cert_directory;

	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cPayload[100];

	int32_t i = 0;

	IoT_Error_t rc = FAILURE;

	AWS_IoT_Client client;
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	IoT_Publish_Message_Params paramsQOS0;
	IoT_Publish_Message_Params paramsQOS1;

	if (argc < 3) {
		std::cout << "Usage: MultiSensorsDeviceSample [aws_connection_info.json] [certificate_file_path]" << std::endl;
		return 0;
	}
	
	AWSIoTConnectionInfo conn_info;

	if (conn_info.load_file(argv[1]) == false) {
		std::cout << "I couldn't parse '" << argv[1] << "' for IoT connection info.'" << std::endl;
		return 0;
	}

	cert_directory = argv[2];

	IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	snprintf(rootCA, PATH_MAX + 1, "%s/%s", cert_directory.c_str(), conn_info.get_root_ca_filename().c_str());
	snprintf(clientCRT, PATH_MAX + 1, "%s/%s", cert_directory.c_str(), conn_info.get_certificate_filename().c_str());
	snprintf(clientKey, PATH_MAX + 1, "%s/%s", cert_directory.c_str(), conn_info.get_private_key_filename().c_str());

	IOT_DEBUG("rootCA %s", rootCA);
	IOT_DEBUG("clientCRT %s", clientCRT);
	IOT_DEBUG("clientKey %s", clientKey);

	mqttInitParams.enableAutoReconnect = true; // We enable this later below
	mqttInitParams.pHostURL = (char*)conn_info.get_mqtt_host().c_str();
	mqttInitParams.port = conn_info.get_mqtt_port();
	mqttInitParams.pRootCALocation = rootCA;
	mqttInitParams.pDeviceCertLocation = clientCRT;
	mqttInitParams.pDevicePrivateKeyLocation = clientKey;
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
	if(SUCCESS != rc) {
		IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
		return rc;
	}

	connectParams.keepAliveIntervalInSec = 10;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = (char *)conn_info.get_mqtt_client_id().c_str();
	connectParams.clientIDLen = conn_info.get_mqtt_client_id().length();
	connectParams.isWillMsgPresent = false;

	IOT_INFO("Connecting...");

	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if(SUCCESS != rc) {
		IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
		return rc;
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if(SUCCESS != rc) {
		IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
		return rc;
	}

	IOT_INFO("Subscribing...");
	

	std::string caller_name1 = "handler for subscribing 'topic_1'";
	std::string caller_name2 = "handler for subscribing 'topic_2'";
	std::string caller_name3 = "handler for subscribing 'topic_3'";

	rc = aws_iot_mqtt_subscribe(&client, "topic_1", 7, QOS0, iot_subscribe_callback_handler, &caller_name1);
	rc = aws_iot_mqtt_subscribe(&client, "topic_2", 7, QOS0, iot_subscribe_callback_handler, &caller_name2);
	rc = aws_iot_mqtt_subscribe(&client, "topic_3", 7, QOS0, iot_subscribe_callback_handler, &caller_name3);

	if(SUCCESS != rc) {
		IOT_ERROR("Error subscribing : %d ", rc);
		return rc;
	}

	sprintf(cPayload, "%s : %d ", "hello from SDK", i);

	paramsQOS0.qos = QOS0;
	paramsQOS0.payload = (void *) cPayload;
	paramsQOS0.isRetained = 0;

	paramsQOS1.qos = QOS1;
	paramsQOS1.payload = (void *) cPayload;
	paramsQOS1.isRetained = 0;

    if (publishCount != 0) {
        infinitePublishFlag = false;
    }

    std::thread t([&] {
        while (true) {
            rc = aws_iot_mqtt_yield(&client, 100);
        }
    });

#ifdef LUNA_PLATFORM_RPI
    setup();

    std::thread t2([&] {
        while (true) {
            loop();
            usleep(1000);
        }
    });

    t2.join(); 
#endif

    t.join();

	if(SUCCESS != rc) {
		IOT_ERROR("An error occurred in the loop.\n");
	} else {
		IOT_INFO("Publish done\n");
	}

	return rc;
}
