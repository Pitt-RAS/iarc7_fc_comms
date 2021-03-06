//////////////////////////////////////////////////////////////////////////
//
// Msp flight controller node.
//
// This node connects to the FC using the MSP protocol.
//
////////////////////////////////////////////////////////////////////////////

#include <ros/ros.h>
#include "iarc7_fc_comms/CommonFcComms.hpp"
#include "iarc7_fc_comms/MspFcComms.hpp"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "FC_comms");

    ROS_DEBUG("FC_comms begin");

    CommonFcComms<MspFcComms>& fc = CommonFcComms<MspFcComms>::getInstance();

    if (fc.init() != FcCommsReturns::kReturnOk) {
        ROS_ERROR("FcComms initialization failed");
        return 1;
    }

    if(fc.run() != FcCommsReturns::kReturnOk)
    {
        ROS_ERROR("FC Node exited with error");
        return 1;
    }

    // All is good.
    return 0;
}
