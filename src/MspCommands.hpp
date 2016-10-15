#ifndef MSP_COMMANDS_HPP
#define MSP_COMMANDS_HPP

////////////////////////////////////////////////////////////////////////////
//
// Msp command packets
//
////////////////////////////////////////////////////////////////////////////
#include "MspConf.hpp"

namespace FcComms
{
    struct MSP_IDENT
    {
        // Default constructor an destructor
        MSP_IDENT() = default;
        ~MSP_IDENT() = default;

        // Don't allow the copy constructor or assignment.
        MSP_IDENT(const MSP_IDENT& rhs) = delete;
        MSP_IDENT& operator=(const MSP_IDENT& rhs) = delete;

        static const uint8_t message_id{100};
        static const uint8_t data_length{0};

        static constexpr char const * const string_name{"MSP_IDENT"};

        const uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];
    };

    struct MSP_RC
    {
        // Default constructor an destructor
        MSP_RC() = default;
        ~MSP_RC() = default;

        // Don't allow the copy constructor or assignment.
        MSP_RC(const MSP_RC& rhs) = delete;
        MSP_RC& operator=(const MSP_RC& rhs) = delete;

        static const uint8_t message_id{105};
        static const uint8_t data_length{0};

        static constexpr char const * const string_name{"MSP_RC"};

        uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];

        void getRc(uint16_t (&rc_values)[18])
        {
            // Jetson runs in little endian mode and the FC
            // Receives data in big endian
            // Jetson sends back 18 channels, two bytes each.
            #pragma GCC warning "Get rid of this hardcoded number"
            for(int i = 0; i < 36; i+=2)
            {
                rc_values[i] = response[i] | (response[i+1] << 8);
            }
        }

        bool getAutoEnabled()
        {
            // Jetson runs in little endian mode and the FC
            // sends data in big endian
            // Yay magic numbers
            uint16_t aux5 = response[16] | (response[17] << 8);
            return aux5 > 1500 ? true : false;
        }
    };

    struct MSP_SET_RAW_RC
    {
        // Default constructor an destructor
        MSP_SET_RAW_RC() = default;
        ~MSP_SET_RAW_RC() = default;

        // Don't allow the copy constructor or assignment.
        MSP_SET_RAW_RC(const MSP_SET_RAW_RC& rhs) = delete;
        MSP_SET_RAW_RC& operator=(const MSP_SET_RAW_RC& rhs) = delete;

        static const uint8_t message_id{200};
        static const uint8_t data_length{8*2}; // 8 channels 2 bytes each

        static constexpr char const * const string_name{"MSP_SET_RAW_RC"};

        uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];

        void packRc(uint16_t (&rc_values)[8])
        {
            // Jetson runs in little endian mode and the FC
            // Receives data in big endian
            uint8_t* data_ptr = reinterpret_cast<uint8_t*>(&rc_values[0]);
            // 8 channels sent, two bytes each
            #pragma GCC warning "Get rid of hardcoded number"
            std::copy(data_ptr, data_ptr+16, send);
        }
    };

    struct MSP_ANALOG
    {
        // Default constructor an destructor
        MSP_ANALOG() = default;
        ~MSP_ANALOG() = default;

        // Don't allow the copy constructor or assignment.
        MSP_ANALOG(const MSP_ANALOG& rhs) = delete;
        MSP_ANALOG& operator=(const MSP_ANALOG& rhs) = delete;

        static const uint8_t message_id{110};
        static const uint8_t data_length{0};

        static constexpr char const * const string_name{"MSP_ANALOG"};

        const uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];

        float getVolts()
        {
            return static_cast<float>(response[0]/10.0);
        }
    };

    struct MSP_STATUS
    {
        // Default constructor an destructor
        MSP_STATUS() = default;
        ~MSP_STATUS() = default;

        // Don't allow the copy constructor or assignment.
        MSP_STATUS(const MSP_STATUS& rhs) = delete;
        MSP_STATUS& operator=(const MSP_STATUS& rhs) = delete;

        static const uint8_t message_id{101};
        static const uint8_t data_length{0};

        static constexpr char const * const string_name{"MSP_STATUS"};

        const uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];

        bool getArmed()
        {
            uint32_t flags{0};
            flags |= static_cast<uint32_t>(response[6]);
            flags |= static_cast<uint32_t>(response[7]) << 8;
            flags |= static_cast<uint32_t>(response[8]) << 16;
            flags |= static_cast<uint32_t>(response[9]) << 24;

            // The armed flag happens to be the first one
            return flags & 0x1;
        }
    };
}

#endif