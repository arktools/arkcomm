/* MAVLink adapterr */

#include "asio_mavlink_bridge.h"

BufferedAsyncSerial * mavlink_comm_0_port;
BufferedAsyncSerial * mavlink_comm_1_port;
BufferedAsyncSerial * mavlink_comm_2_port;
mavlink_system_t mavlink_system = {1,1,0,0};
