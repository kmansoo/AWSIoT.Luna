/*
 * AWSIoTConnectionInfo.cpp
 *
 *  Created on: 2016. 9. 6.
 *      Author: Mansoo Kim
 */

#include <fstream>
#include <iostream>

#include "ccJsonParser/json/reader.h"

#include "AWSIoTConnectionInfo.h"

bool AWSIoTConnectionInfo::load_file(const std::string& filename) {
    std::ifstream ifs(filename);

    if (!ifs) {
        std::cout << "Couldn't open this file: " << filename << std::endl;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    Json::Reader    reader;
    Json::Value     root_value;

    if (!reader.parse(content, root_value)) {
        std::cout << "step1" << std::endl;
        return false;
    }

    if (root_value.isObject() == false) {
        std::cout << "step2" << std::endl;
        return false;
    }

    if (root_value["AWSIoTConnectionInfo"].isNull()) {
        std::cout << "step3" << std::endl;
        return false;
    }

    if (!root_value["AWSIoTConnectionInfo"]["mqtt_host"].isNull())
        mqtt_host_ = root_value["AWSIoTConnectionInfo"]["mqtt_host"].asString();

    if (!root_value["AWSIoTConnectionInfo"]["mqtt_port"].isNull())
        mqtt_port_ = root_value["AWSIoTConnectionInfo"]["mqtt_port"].asUInt();

    if (!root_value["AWSIoTConnectionInfo"]["mqtt_client_id"].isNull())
        mqtt_client_id_ = root_value["AWSIoTConnectionInfo"]["mqtt_client_id"].asString();

    if (!root_value["AWSIoTConnectionInfo"]["my_thing_name"].isNull())
        my_thing_name_ = root_value["AWSIoTConnectionInfo"]["my_thing_name"].asString();

    if (!root_value["AWSIoTConnectionInfo"]["root_ca_filename"].isNull())
        root_ca_filename_ = root_value["AWSIoTConnectionInfo"]["root_ca_filename"].asString();

    if (!root_value["AWSIoTConnectionInfo"]["certificate_filename"].isNull())
        certificate_filename_ = root_value["AWSIoTConnectionInfo"]["certificate_filename"].asString();

    if (!root_value["AWSIoTConnectionInfo"]["private_key_filename"].isNull())
        private_key_filename_ = root_value["AWSIoTConnectionInfo"]["private_key_filename"].asString();

    return true;
}
