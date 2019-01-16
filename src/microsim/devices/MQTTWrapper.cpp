/****************************************************************************/
/// @file    MQTTWrapper.cpp
/// @author  Bruno Coudoin
/// @date    29.04.2017
/// @version $Id: MQTTWrapper.cpp $
///
// A Wrapper for MQTT
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 200132014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <string>
#include <iostream> // For debug traces only

#include "MQTTWrapper.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

// ===========================================================================
// MQTT class definition
// ===========================================================================

MQTTWrapper& MQTTWrapper::Instance() {
    return m_instance;
}

MQTTWrapper::MQTTWrapper() {
}


MQTTWrapper::~MQTTWrapper() {
    loop_stop();              // Kill the thread
    mosqpp::lib_cleanup();    // Mosquitto library cleanup
}

// @return true if init succeeded
bool MQTTWrapper::init(const std::string& id, const std::string& host, int port, bool using_credentials)
{
    if(init_count++ == 0) {
        std::cout << "MQTT init " << id << std::endl;
        _id = id;
        int keepalive = 120;                // seconds
        mosqpp::lib_init();			        // Initialize libmosquitto

        reinitialise(id.c_str(), true);     // Pass the id now
        std::string cafile = "/opt/sumo/credentials/rootca.pem";

        if (using_credentials) {
            // Check for root CA
            if (FILE *file = fopen(cafile.c_str(), "r")) {
                fclose(file);
            } else {
                std::cout << "WARNING: Could not find certificate authority file at '/opt/sumo/credentials/rootca.pem'!" << std::endl;
            }

            // Check for device certificate
            std::string certfile = "/opt/sumo/credentials/device.pem";
            if (FILE *file = fopen(certfile.c_str(), "r")) {
                fclose(file);
            } else {
                std::cout << "WARNING: Could not find device certificate at '/opt/sumo/credentials/device.pem'!" << std::endl;
            }

            // Check for device key
            std::string keyfile = "/opt/sumo/credentials/device.key";
            if (FILE *file = fopen(keyfile.c_str(), "r")) {
                fclose(file);
            } else {
                std::cout << "WARNING: Could not find device key at '/opt/sumo/credentials/device.key'!" << std::endl;
            }

            // Load the credentials
            tls_set(cafile.c_str(), (const char*)NULL, certfile.c_str(), keyfile.c_str(), nullptr);
        }

        int retval = connect(host.c_str(), port, keepalive); // Connect to MQTT Broker
        if(retval) {
            std::cout << "TRIED CONNECT: " << _id << "; RETURNED: " << mosqpp::strerror(retval) << std::endl;
            return false;
        } else {
            // This create a communication thread, no need to loop to pass messages
            loop_start();
        }
    }
    return true;
}

void MQTTWrapper::deinit()
{
    // We maintain a reference count and just deinit when the last
    // user calls deinit.
    if(--init_count == 0) {
        std::cout << "MQTT deinit " << std::endl;
        loop_stop(true);          // Kill the thread
        mosqpp::lib_cleanup();    // Mosquitto library cleanup
        init_count = 0;
    }
}

void MQTTWrapper::on_connect(int rc)
{
    std::cout << "MQTT Connected with code " << rc << std::endl;
}

void MQTTWrapper::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    std::cout << "MQTT Subscription succeeded mid=" << mid << std::endl;
}

bool MQTTWrapper::do_subscribe(const std::string &topic) {
    int res = subscribe(NULL, topic.c_str(), 0);
    std::cout << "MQTT subscribe to " << topic << " with code " << res << std::endl;
}

bool MQTTWrapper::do_unsubscribe(const std::string &topic) {
    int res = unsubscribe(NULL, topic.c_str());
    std::cout << "MQTT unsubscribe to " << topic << " with code " << res << std::endl;
}

// write the given data to the MQTT server
// @returns true on success
bool MQTTWrapper::write_message(const std::string &topic, const std::string &output_message) {
    int retval =   publish(NULL, topic.c_str(), output_message.length(), output_message.c_str(), 0 , 0);
    if(retval) {
        std::cout << "publish returned: " << mosqpp::strerror(retval) << std::endl;
        return false;
    }
    return true;
}
