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
        static const bool has_response{true};

        static constexpr char const * const string_name{"MSP_IDENT"};

        const uint8_t send[FcCommsMspConf::kMspMaxDataLength]={};

        uint8_t response[FcCommsMspConf::kMspMaxDataLength];
    };
}

#endif