#ifndef COMMON_FC_COMMS_HPP
#define COMMON_FC_COMMS_HPP

////////////////////////////////////////////////////////////////////////////
//
// Common flight controller node.
//
// Contains declarations for objects common to all flight
// control communication nodes. Handles a lot of ROS specifics.
//
////////////////////////////////////////////////////////////////////////////
#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "iarc7_msgs/FlightControllerStatus.h"
#include "CommonConf.hpp"

namespace FcComms{

    // Used for the result of a command
    enum class FcCommsReturns
    {
        kReturnOk,
        kReturnError
    };

    // Used for the current state of communication with flight controller
    enum class FcCommsStatus
    {
        kDisconnected,
        kConnected,
        kConnecting
    };

    template<class T>
    class CommonFcComms
    {
    public:
        // Used to make sure this class remains a singleton.
        static CommonFcComms<T>& getInstance();

        FcCommsReturns init();
        
        FcCommsReturns run(int argc, char **argv);

        //Delete copy constructors, assignment operator
        CommonFcComms(const CommonFcComms& rhs) = delete;
        CommonFcComms& operator=(const CommonFcComms& rhs) = delete;

    private:

        // Class can only be made by class
        CommonFcComms() = default;

        // This nodes handle.
        ros::NodeHandle nh_; 

        // Publishers for FC sensors
        ros::Publisher battery_publisher;
        ros::Publisher status_publisher;

        // Publish to the FC sensor topics
        void publishTopics();

        // Callback to update the sensors on the FC
        void updateSensors(const ros::TimerEvent&);

        // Just use the default constructor
        T flightControlImpl_;
    };
}

using namespace FcComms;

template<class T>
CommonFcComms<T>& CommonFcComms<T>::getInstance()
{
    // Allow only one instance
    static CommonFcComms<T>* instance = nullptr;
    if (instance == nullptr) {
        instance = new CommonFcComms<T>;
    }
    return *instance;
}

// Use to connect to topics
template<class T>
FcCommsReturns CommonFcComms<T>::init()
{
    // Should error check or something

    battery_publisher = nh_.advertise<std_msgs::Float32>("fc_battery", 50);
    status_publisher = nh_.advertise<iarc7_msgs::FlightControllerStatus>("fc_status", 50);

    flightControlImpl_.subscribeControl(nh_);

    ROS_INFO("FC Comms registered and subscribed to topics");

    return FcCommsReturns::kReturnOk;
}

// Main run loop of node.
template<class T>
FcCommsReturns CommonFcComms<T>::run(int argc, char **argv)
{

    // Set up a 10hz Timer to call updateSensors
    ros::Timer timer = nh_.createTimer(ros::Duration(CommonConf::kFcSensorsUpdatePeriod), &CommonFcComms::updateSensors, this);

    // Wait till we get told to stop and service all callbacks
    ros::spin();

    // Disconnect from FC.
    flightControlImpl_.disconnect();

    return FcCommsReturns::kReturnOk;
}

// Push the sensor data to the appropriate topics.
template<class T>
void CommonFcComms<T>::publishTopics()
{
    iarc7_msgs::FlightControllerStatus fc;
    flightControlImpl_.getStatus(fc.armed, fc.auto_pilot, fc.failsafe);
    
    std_msgs::Float32 battery;
    flightControlImpl_.getBattery(battery.data);

    status_publisher.publish(fc);
    battery_publisher.publish(battery);
}

// Update the sensors on the flight controller
template<class T>
void CommonFcComms<T>::updateSensors(const ros::TimerEvent&)
{
    // Do different things based on the current connection status.
    switch(flightControlImpl_.getConnectionStatus())
    {
        case FcCommsStatus::kDisconnected:
            ROS_WARN("FC_Comms disconnected");
            flightControlImpl_.connect();
            ROS_INFO("Back from connect");
            break;
        
        case FcCommsStatus::kConnected:
            ROS_INFO("FC_comms updating FC sensors");
            flightControlImpl_.handleComms();
            publishTopics();
            break;

        case FcCommsStatus::kConnecting:
            break;

        default:
            ROS_ERROR("FC_Comms has undefined state.");
    }
}

#endif