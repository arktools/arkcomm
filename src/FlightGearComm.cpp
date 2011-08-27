/*
 * FlightGearComm.cpp
 * Copyright (C) James Goppert 2010 <jgoppert@users.sourceforge.net>
 *
 * FlightGearComm.cpp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FlightGearComm.cpp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FlightGearComm.hpp"

FlightGearComm::FlightGearComm(const std::string host, const std::string fdmRecvPort,
                               const std::string ctrlsRecvPort, const std::string ctrlsTransPort) :
    myFdmRecvSock(new SGSocketUDP(host,fdmRecvPort)),
    myCtrlsRecvSock(new SGSocketUDP(host,ctrlsRecvPort)),
    myCtrlsTransSock(new SGSocketUDP(host,ctrlsTransPort)),
    myFdmRecvBuf(),
    myCtrlsRecvBuf(),
    myCtrlsTransBuf()
{
    myFdmRecvSock->open(SG_IO_IN);
    myCtrlsRecvSock->open(SG_IO_IN);
    myCtrlsTransSock->open(SG_IO_OUT);
    std::cout << "FlightGearComm initialized." << std::endl;
}

FlightGearComm::~FlightGearComm()
{
    if (myFdmRecvSock)
    {
        myFdmRecvSock->close();
        delete myFdmRecvSock;
    }
    if (myCtrlsRecvSock)
    {
        myCtrlsRecvSock->close();
        delete myCtrlsRecvSock;
    }
    if (myCtrlsTransSock)
    {
        myCtrlsTransSock->close();
        delete myCtrlsTransSock;
    }
    std::cout << "FlightGearComm closed." << std::endl;
}

void FlightGearComm::receive()
{
    myFdmRecvSock->read( (char*)&myFdmRecvBuf, sizeof(myFdmRecvBuf) );
    htonFdm(myFdmRecvBuf);
    myCtrlsRecvSock->read( (char*)&myCtrlsRecvBuf, sizeof(myCtrlsRecvBuf) );
    htonCtrls(myCtrlsRecvBuf);
    myCtrlsTransBuf = myCtrlsRecvBuf;
}

void FlightGearComm::send()
{
    myCtrlsTransSock->write( (char*)&myCtrlsTransBuf, sizeof(myCtrlsTransBuf) );
}

const FGNetFDM & FlightGearComm::getFdmRecvBuf() const
{
    return myFdmRecvBuf;
}

const FGNetCtrls & FlightGearComm::getCtrlsRecvBuf() const
{
    return myCtrlsRecvBuf;
}

FGNetCtrls & FlightGearComm::setCtrlsTransBuf()
{
    return myCtrlsTransBuf;
}

void FlightGearComm::output() const
{
    printf("%10.6fN (deg) ", getFdmRecvBuf().latitude*rad2deg);
    printf("%10.6fE (deg) ",getFdmRecvBuf().longitude*rad2deg);
    printf("ASL (m)=% 7.1f\n", getFdmRecvBuf().altitude);
    printf("phi(deg)=% 3.0f   ", getFdmRecvBuf().phi*rad2deg);
    printf("theta(deg)=% 3.0f   ", getFdmRecvBuf().theta*rad2deg);
    printf("psi(deg)=% 3.0f\n", getFdmRecvBuf().psi*rad2deg);
    printf("phidot(deg/s)=% 3.0f   ", getFdmRecvBuf().phidot*rad2deg);
    printf("thetadot(deg/s)=% 3.0f   ", getFdmRecvBuf().thetadot*rad2deg);
    printf("psidot(deg/s)=% 3.0f\n", getFdmRecvBuf().psidot*rad2deg);
    printf("vcas(knots)=% 3.0f\n", getFdmRecvBuf().vcas);
    printf("Abody (m/s^2) = %10.4f %10.4f %10.4f\n",getFdmRecvBuf().A_X_pilot*ft2m,getFdmRecvBuf().A_Y_pilot*ft2m,getFdmRecvBuf().A_Z_pilot*ft2m);
    printf("alpha(deg)=% 3.0f   ", getFdmRecvBuf().alpha*rad2deg);
    printf("beta(deg)=% 3.0f   ", getFdmRecvBuf().beta*rad2deg);
    printf("\n");
}

// vim:ts=4:sw=4
