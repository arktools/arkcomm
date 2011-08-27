/* MAVLink adapter header */
#ifndef APM_MAVLINK_BRIDGE__H
#define APM_MAVLINK_BRIDGE__H

#include "mavlink_types.h"
#include "AsyncSerial.hpp"

extern BufferedAsyncSerial * mavlink_comm_0_port;
extern BufferedAsyncSerial * mavlink_comm_1_port;
extern BufferedAsyncSerial * mavlink_comm_2_port;
extern mavlink_system_t mavlink_system; 

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void comm_send_ch(mavlink_channel_t chan, uint8_t ch)
{
    switch(chan)
    {
    case MAVLINK_COMM_0:
        mavlink_comm_0_port->write((const char*) &ch, 1);
        break;
	case MAVLINK_COMM_1:
        mavlink_comm_1_port->write((const char*) &ch, 1);
        break;
	case MAVLINK_COMM_2:
        mavlink_comm_2_port->write((const char*) &ch, 1);
        break;
    }
}

static inline uint8_t comm_receive_ch(mavlink_channel_t chan)
{
    uint8_t data = 0;
    switch(chan)
    {
    case MAVLINK_COMM_0:
        while(!mavlink_comm_0_port->read((char*) &data,1));
        break;
	case MAVLINK_COMM_1:
        while(!mavlink_comm_1_port->read((char*) &data,1));
        break;
	case MAVLINK_COMM_2:
        while(!mavlink_comm_2_port->read((char*) &data,1));
        break;
    }
    return data;
}

static inline uint16_t comm_get_available(mavlink_channel_t chan)
{
    uint16_t bytes = 0;
    switch(chan)
    {
    case MAVLINK_COMM_0:
        bytes = mavlink_comm_0_port->available();
        break;
	case MAVLINK_COMM_1:
        bytes = mavlink_comm_1_port->available();
        break;
	case MAVLINK_COMM_2:
        bytes = mavlink_comm_2_port->available();
        break;
    }
    return bytes;
}
#endif /* APM_MAVLINK_BRIDGE__H */
