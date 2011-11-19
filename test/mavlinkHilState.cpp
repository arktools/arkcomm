#include "AsyncSerial.hpp"
#include <iostream>
#include <stdexcept>
#include <ctime>

// mavlink system definition and headers
#include "mavlink/mavlink_types.h"
#include "asio_mavlink_bridge.h"
#include "mavlink/common/mavlink.h"

static const double rad2deg = 180.0/3.14159;

class MavlinkHilState {

private:
    uint16_t count;
    uint16_t packet_drops;
    double hilTimeStamp;
    double time;
    uint16_t hilRate;
    mavlink_channel_t chan;

public:
    MavlinkHilState(const std::string & device, uint16_t baudRate) : count(0), packet_drops(0), hilTimeStamp(0), time(0), hilRate(50), chan(MAVLINK_COMM_0) {
        if (mavlink_comm_0_port == NULL)
        {
            try
            {
                mavlink_comm_0_port = new BufferedAsyncSerial(device,baudRate);
            }
            catch(const boost::system::system_error & e)
            {
                std::cout << "error: " << e.what() << std::endl;
            }
        }
    }

    ~MavlinkHilState() {
        if (mavlink_comm_0_port)
        {
            delete mavlink_comm_0_port;
            mavlink_comm_0_port = NULL;
        }
    }
    
    void update() {
        // send attitude message
        if (time - hilTimeStamp > 1.0/hilRate)
        {
            hilTimeStamp = time;

            // attitude states (rad)
            float roll = 1;
            float pitch = 2;
            float yaw = 3;

            // body rates
            float rollRate = 0.1;
            float pitchRate = 0.2;
            float yawRate = 0.3;

            // position
            int32_t lat = 1*rad2deg*1e7;
            int32_t lon = 2*rad2deg*1e7;
            int16_t alt = 3*1e3;

            int16_t vx = 1*1e2;
            int16_t vy = 2*1e2;
            int16_t vz = -3*1e2;

            int16_t xacc = 1*1e3;
            int16_t yacc = 2*1e3;
            int16_t zacc = 3*1e3;

            mavlink_msg_hil_state_send(chan,hilTimeStamp,
                                       roll,pitch,yaw,
                                       rollRate,pitchRate,yawRate,
                                       lat,lon,alt,
                                       vx,vy,vz,
                                       xacc,yacc,zacc);
        }
        else if (time  - hilTimeStamp < 0)
            hilTimeStamp = time;


        // receive messages
        mavlink_message_t msg;
        mavlink_status_t status;

        while(comm_get_available(MAVLINK_COMM_0))
        {
            uint8_t c = comm_receive_ch(MAVLINK_COMM_0);

            // try to get new message
            if(mavlink_parse_char(MAVLINK_COMM_0,c,&msg,&status))
            {
                switch(msg.msgid)
                {

                case MAVLINK_MSG_ID_HIL_CONTROLS:
                {
                    //std::cout << "receiving messages" << std::endl;
                    mavlink_hil_controls_t hil_controls;
                    mavlink_msg_hil_controls_decode(&msg,&hil_controls);
                    //y[0] = hil_controls.roll_ailerons;
                    //y[1] = hil_controls.pitch_elevator;
                    //y[2] = hil_controls.yaw_rudder;
                    //y[3] = hil_controls.throttle;
                    //y[4] = hil_controls.mode;
                    //y[5] = hil_controls.nav_mode;
                    break;
                }

                }
            }

            // update packet drop counter
            packet_drops += status.packet_rx_drop_count;
        }
    }
};

int main (int argc, char const* argv[])
{
    if (argc != 2) {
        std::cout << "usage: mavlinkState device baud" << std::endl;
        return 1;
    };
    MavlinkHilState test(argv[1],atoi(argv[2])); 
    while(1) {
        test.update();
        sleep(1);
    }
    return 0;
}

// vim:ts=4:sw=4:expandtab
