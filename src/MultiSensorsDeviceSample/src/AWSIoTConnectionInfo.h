/*
 * AWSIoTConnectionInfo.h
 *
 *  Created on: 2016. 9. 6.
 *      Author: Mansoo Kim
 */

#ifndef __AWS_IOT_CONNECTION_INFO_H__
#define __AWS_IOT_CONNECTION_INFO_H__

#include <string>

// {
//     "AWSIoTConnectionInfo": {
//         "mqtt_host": "xxxx.iot.us-west-x.amazonaws.com",
//         "mqtt_port": 8883,
//         "mqtt_client_id" : "client_id_01",
//         "my_thing_name" : "thing_name",
//         "root_ca_filename" : "xxx-rootCA.crt",
//         "certificate_filename" : "xxx-certificate.pem.crt",
//         "private_key_filename" : "xxx-private.pem.key"
//     }
// }

class AWSIoTConnectionInfo {
public:
    AWSIoTConnectionInfo() {}

public:
    bool    load_file(const std::string& filename);

    const std::string&  get_mqtt_host() { return mqtt_host_; }
    const std::uint16_t get_mqtt_port() { return mqtt_port_; }
    const std::string&  get_mqtt_client_id() { return mqtt_client_id_; }
    const std::string&  get_my_thing_name() { return my_thing_name_; }
    const std::string&  get_root_ca_filename() { return root_ca_filename_; }
    const std::string&  get_certificate_filename() { return certificate_filename_; }
    const std::string&  get_private_key_filename() { return private_key_filename_; }

private:
    std::string mqtt_host_;
    std::uint16_t mqtt_port_;
    std::string mqtt_client_id_;
    std::string my_thing_name_;
    std::string root_ca_filename_;
    std::string certificate_filename_;
    std::string private_key_filename_;
};

#endif
