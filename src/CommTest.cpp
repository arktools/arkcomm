#include <simgear/debug/logstream.hxx>
#include <simgear/io/iochannel.hxx>
#include <simgear/io/sg_serial.hxx>
#include <simgear/io/sg_socket.hxx>
#include <simgear/io/sg_socket_udp.hxx>
#include <simgear/math/sg_types.hxx>
#include <simgear/timing/timestamp.hxx>
#include "protocol.hxx"
#include <iostream>
#include <cstdlib>

class MAVLinkProtocol : public FGProtocol {
    bool open() {
    }

    bool process() {
    } 

    bool close() {
    }

    bool gen_message() {
    }

    bool parse_message() {
    }
};

int main (int argc, char const* argv[])
{
    MAVLinkProtocol test;

    SGIOChannel * channel;

    if (argc != 3) {
        std::cout << "usage: " << argv[0] 
            << " host port" << std::endl;
        exit(1);
    }
    try {
        channel = new SGSocketUDP(argv[1],argv[2]);
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
    }

    //channel.write(buf, len);

    return 0;
}

