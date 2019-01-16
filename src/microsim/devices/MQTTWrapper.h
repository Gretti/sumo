/****************************************************************************/
/// @file    MQTTWrapper.h
/// @author  Bruno Coudoin
/// @date    29.04.2017
/// @version $Id: MQTTWrapper.h $
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
#ifndef MQTTWrapper_h
#define MQTTWrapper_h

#include <string>
#include <mosquittopp.h>


// ===========================================================================
// MQTT class definition
// ===========================================================================

class MQTTWrapper : public mosqpp::mosquittopp {
private:
    std::string _id;
    int init_count;
    static MQTTWrapper m_instance;

//    // NULLPTR emulator definition
//    const                        // this is a const object...
//    class {
//    public:
//      template<class T>          // convertible to any type
//        operator T*() const      // of null non-member
//        { return 0; }            // pointer...
//      template<class C, class T> // or any type of null
//        operator T C::*() const  // member pointer...
//        { return 0; }
//    private:
//      void operator&() const;    // whose address can't be taken
//    } nullptr = {};              // and whose name is nullptr
//    
public:
    static MQTTWrapper& Instance();
    MQTTWrapper();
    virtual ~MQTTWrapper();

    // @return true if init succeeded
    bool init(const std::string& id, const std::string& host, int port, bool using_credentials);
    void deinit();
    virtual void on_connect(int rc);
    virtual void on_subscribe(int mid, int qos_count, const int *granted_qos);
    bool do_subscribe(const std::string &topic);
    bool do_unsubscribe(const std::string &topic);

    // write the given data to the MQTT server
    // @returns true on success
    bool write_message(const std::string &topic, const std::string &output_message);

};

#endif
