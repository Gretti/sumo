/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_DBWriter.cpp
/// @author  Gretti N'guessan
/// @date    17.12.2018
/// @version 0
///
// A device which stands as an implementation sensoris and which outputs movereminder calls
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include <iostream>
#include <ctime>
#include <stdlib.h>     /* srand, rand */

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_DBWriter.h"

// Sensoris includes
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

// SQLite3 include
//#include <sqlite3.h>

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------

//MQTTWrapper MQTTWrapper::m_instance = MQTTWrapper();

void MSDevice_DBWriter::insertOptions(OptionsCont &oc) {
    oc.addOptionSubTopic("Sensoris Builder Device");
    insertDefaultAssignmentOptions("sensoris", "Sensoris Builder Device", oc);
}

void MSDevice_DBWriter::buildVehicleDevices(SUMOVehicle &v, std::vector<MSVehicleDevice *> &into) {
    OptionsCont &oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "sensoris", v, false)) {
        // build the device
        MSDevice_DBWriter *device = new MSDevice_DBWriter(v, "sumo_" + v.getID());
        into.push_back(device);
    }
}

// ---------------------------------------------------------------------------
// MSDevice_DBWriter-methods
// ---------------------------------------------------------------------------

MSDevice_DBWriter::MSDevice_DBWriter(SUMOVehicle &holder, const std::string &id) : MSVehicleDevice(holder, id) {
    // Fill Sensoris invariant fields
    sensoris.mutable_envelope()->set_version("2.0.2.3");
    sensoris.mutable_envelope()->mutable_vehiclemetadata()->set_vehicletypegeneric(::VehicleMetaData_VehicleTypeGenericEnum_PASSENGER_CAR);
    sensoris.mutable_envelope()->mutable_vehiclemetadata()->set_vehiclereferencepointdeltaaboveground_m(0.5);
    sensoris.mutable_envelope()->set_submitter(getID());

    //Open SQLite DB to write data in
//    sqlite3_open(":memory:", &_db);

    std::cout << "initialized device '" << id << "'\n";
}

MSDevice_DBWriter::~MSDevice_DBWriter() {
//    sqlite3_close(_db);
}

void MSDevice_DBWriter::computeLength(double &routeLength) const {
    double finalPos;
    double finalPosOnInternal = 0;
    finalPos = myHolder.getPositionOnLane();
    if (!MSGlobals::gUseMesoSim) {
        const MSLane *lane = static_cast<MSVehicle &> (myHolder).getLane();
        if (lane->getEdge().isInternal()) {
            finalPosOnInternal = finalPos;
            finalPos = myHolder.getEdge()->getLength();
        }
    }
    const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
    routeLength = myHolder.getRoute().getDistanceBetween(myHolder.getDepartPos(), finalPos,
            myHolder.getRoute().begin(), myHolder.getCurrentRouteEdge(), includeInternalLengths) +
            finalPosOnInternal;
}

void MSDevice_DBWriter::toGeo(Position &x) {
    GeoConvHelper::getFinal().cartesian2geo(x);
}

void MSDevice_DBWriter::computeSensoris(Message* sensoris) const {
    sensoris->mutable_envelope()->set_transientvehicleid(startTimestamp);
    //Cycle through position list to build Sensoris positions
    for (posInfo p : positions) {
        // Create sensoris message
        PositionEstimate *positionEstimate = sensoris->mutable_path()->add_positionestimate();
        positionEstimate->set_altitude_m(p.p.z());
        positionEstimate->set_latitude_deg(p.p.y());
        positionEstimate->set_longitude_deg(p.p.x());
        positionEstimate->set_heading_deg((-p.a + M_PI / 2) * 180 / M_PI);
        positionEstimate->set_positiontype(::PositionEstimate_PositionTypeEnum_FILTERED);
        positionEstimate->set_horizontalaccuracy_m(2);
        positionEstimate->set_timestamputc_ms(p.t);
        positionEstimate->set_speed_mps(p.s);

        // Serialize to protobuf
        std::string bin_output;
        sensoris->SerializeToString(&bin_output);
    }
}

bool MSDevice_DBWriter::notifyEnter(SUMOVehicle &veh, MSMoveReminder::Notification reason, const MSLane * /* enteredLane */) {
    // Subscribe the client to the vehicle's topic when entering the network
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        // startTimestamp = std::time(nullptr);
        startTimestamp = MSNet::getInstance()->getCurrentTimeStep();
        // std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " startTime=" << std::asctime(std::localtime(&startTimestamp)) << "\n";
    }
    return true; // keep the device
}

bool MSDevice_DBWriter::notifyLeave(SUMOVehicle &veh, double /*lastPos*/, MSMoveReminder::Notification reason, const MSLane * /* enteredLane */) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED && positions.size() > 1) {
        computeSensoris(&sensoris);
    }
    //Close SQLite connexion

    return true; // keep the device
}

bool MSDevice_DBWriter::notifyMove(SUMOVehicle &veh, double /*oldPos*/, double /*newPos*/, double /*newSpeed*/) {
    //std out float precision
    std::cout.precision(8);

    //Instanciate a route length counter to hold snippets
    double routeLength;

    //Compute vehicule position in WGS84
    posInfo pi;
    Position pos = veh.getPosition();
    toGeo(pos);
    pi.p = pos;
    // pi.t = std::time(nullptr);
    pi.t = MSNet::getInstance()->getCurrentTimeStep();
    pi.s = veh.getSpeed();
    pi.a = (-veh.getAngle() + M_PI / 2) * 180 / M_PI;
    //Store current position
    positions.push_back(pi);
    //Increment route length
    computeLength(routeLength);
    cumLength += (routeLength - lastLength);
    lastLength = routeLength;
    //If length exceeds 100m compute a Sensoris snippet
    if (cumLength >= 100.0) {
        // Fill Sensoris invariant fields
        sensoris.mutable_envelope()->set_transientvehicleid(startTimestamp);

        //Cycle through position list to build Sensoris positions
        computeSensoris(&sensoris);
        /*//:::DEBUG:::
        if (getID() == "sumo_veh274") {
            std::string plain_text;
            google::protobuf::TextFormat::PrintToString(sensoris, &plain_text);
            std::cout << plain_text << std::endl;
        }
        //:::END DEBUG:::*/
        // Serialize to protobuf
        std::string bin_output;
        sensoris.SerializeToString(&bin_output);

        //Reset all tracking variables
        startTimestamp = MSNet::getInstance()->getCurrentTimeStep() + rand() % 1000;
        lastLengthTrigger = routeLength;
        cumLength = 0;
        positions.clear();
        positions.push_back(pi);
        // Clear expired data
        sensoris.mutable_path()->clear_positionestimate();
        sensoris.clear_pathevents();
    }

    return true; // keep the device
}

void MSDevice_DBWriter::generateOutput() const {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        OutputDevice& os = OutputDevice::getDeviceByOption("tripinfo-output");
        os.openTag("sensoris_device");
        os.closeTag();
    }
}

std::string MSDevice_DBWriter::getParameter(const std::string &key) const {
    if (key == "meaningOfLife") {
        return "42";
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}

void MSDevice_DBWriter::setParameter(const std::string &key, const std::string &value) {
}

/****************************************************************************/
