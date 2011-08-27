/*
 * FlightGearComm.hpp
 * Copyright (C) James Goppert 2010 <jgoppert@users.sourceforge.net>
 *
 * FlightGearComm.hpp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FlightGearComm.hpp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef mavsim_FlightGearComm_HPP
#define mavsim_FlightGearComm_HPP


#include <stdio.h>
#include <iostream>
#include <math.h>
#define SG_HAVE_STD_INCLUDES
#include <simgear/io/sg_socket_udp.hxx>
#include "flightGearUtils.hpp"

const static float ft2m = 1./3.2808399;
const static float rad2deg = 180/M_PI;
const static float deg2rad = 1./rad2deg;

class FlightGearComm
{
public:
    FlightGearComm(const std::string host, const std::string fdmRecvPort,
                   const std::string ctrlsRecvPort, const std::string ctrlsTransPort);
    ~FlightGearComm();
    void receive();
    void send();
    const FGNetFDM & getFdmRecvBuf() const;
    const FGNetCtrls & getCtrlsRecvBuf() const;
    FGNetCtrls & setCtrlsTransBuf();
    void output() const;
private:
    SGIOChannel * myFdmRecvSock;
    SGIOChannel * myCtrlsRecvSock;
    SGIOChannel * myCtrlsTransSock;
    FGNetFDM myFdmRecvBuf;
    FGNetCtrls myCtrlsRecvBuf;
    FGNetCtrls myCtrlsTransBuf;
};

#endif

// vim:ts=4:sw=4

