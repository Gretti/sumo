/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_DBWriter.h
/// @author  Gretti N'guessan
/// @date    17.12.2018
/// @version 0
///
// A device which stands as an implementation dbwriter and which outputs movereminder calls
/****************************************************************************/
#ifndef MSDevice_DBWriter_h
#define MSDevice_DBWriter_h

// ===========================================================================
// included modules
// ===========================================================================
#include <iostream>
#include <config.h>
#include <list>
#include <ctime>

#include "MSVehicleDevice.h"
#include <utils/common/SUMOTime.h>

//#include "MQTTWrapper.h"

//Sensoris includes
#include <microsim/devices/sensoris/sensoris.pb.h>

// SQLite3 include
//#include <sqlite3.h>

//Structure containing PositionEstimate information

struct posInfo {
    Position p;
    double a;
    SUMOTime t;
    double s;
    // std::time_t t;
};

// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class MSDevice_DBWriter
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSDevice
 */
class MSDevice_DBWriter : public MSVehicleDevice {
public:

//    /**
//     * @brief MQTTClient let us communicates with the MQTT server
//     */
//    MQTTWrapper *MQTTClient;

    /**
     * @brief Sensoris message
     */
    Message sensoris;

    /** @brief Inserts MSDevice_DBWriter-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont &oc);

    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a dbwriter-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle &v, std::vector<MSVehicleDevice *> &into);

public:
    /// @brief Destructor.
    ~MSDevice_DBWriter();

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks for waiting steps when the vehicle moves
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return True (always).
     */
    bool notifyMove(SUMOVehicle &veh, double oldPos,
            double newPos, double newSpeed);

    /** @brief Saves departure info on insertion
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle &veh, MSMoveReminder::Notification reason, const MSLane *enteredLane = 0);

    /** @brief Saves arrival info
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @return True if it did not leave the net.
     */
    bool notifyLeave(SUMOVehicle &veh, double lastPos, MSMoveReminder::Notification reason, const MSLane *enteredLane = 0);

    /// @}

    /// @brief return the name for this type of device

    const std::string deviceName() const {
        return "sensoris";
    }

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string &key) const;

    /// @brief try to set the given parameter for this device. Throw exception for unsupported key
    void setParameter(const std::string &key, const std::string &value);

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput() const;

private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_DBWriter(SUMOVehicle &holder, const std::string &id);
//            ,const std::string& host, int port, bool using_credentials);

    /// @brief compute trip length ///
    void computeLength(double &routeLength) const;

    /// @brief compute a sensoris snippet message ///
    void computeSensoris(Message* sensoris) const;

    /// @brief convert SUMO-positions to geo coordinates (in place)
    static void toGeo(Position &x);

private:
    // private state members of the Database device

    /// @brief snippet start timestamp
    // std::time_t startTimestamp;
    SUMOTime startTimestamp;

    double cumLength = 0;
    double lastLength = 0;
    double lastLengthTrigger;
    //  posInfo lastPos = NULL;
    std::list<posInfo> positions;
    posInfo lastPos;
//    sqlite3* _db;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_DBWriter(const MSDevice_DBWriter &);

    /// @brief Invalidated assignment operator.
    MSDevice_DBWriter &operator=(const MSDevice_DBWriter &);
};

#endif

/****************************************************************************/
