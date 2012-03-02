// multiplay.hxx -- protocol object for multiplay in Flightgear
//
// Written by Diarmuid Tyson, started February 2003.
// diarmuid.tyson@airservicesaustralia.com
//
// With additions by Vivian Meazza, January 2006
//
// Copyright (C) 2003  Airservices Australia
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#ifndef _FG_MULTIPLAY_HXX
#define _FG_MULTIPLAY_HXX

#define FG_MULTIPLAY_HID "$Id$"

#include <simgear/compiler.h>

#include <string>

#include <simgear/props/props.hxx>

//#include <Main/globals.hxx>
//#include <Main/fg_props.hxx>
//#include <Model/acmodel.hxx>
#include "multiplaymgr.hxx"

//#include "protocol.hxx"

using std::string;


/****************************************************************
* @version $Id$
*
* Description: FGMultiplay is an FGProtocol object used as the basic
* interface for the multiplayer code into FlightGears generic IO
* subsystem.  It only implements the basic FGProtocol methods: open(),
* process() and close().  It does not use Sim Gear's IO channels, as
* the MultiplayMgrs creates their own sockets through plib.
*
* It will set up it's direction and rate protocol properties when
* created.  Subsequent calls to process will prompt the
* MultiplayMgr to either send or receive data over the network.
*
******************************************************************/

class FGMultiplay /*: public FGProtocol*/ {
public:

    /** Constructor */
    FGMultiplay (FGMultiplayMgr * mgr, const string dir, const int rate, const string &host, const int port);

    /** Destructor. */
    ~FGMultiplay ();

    /** Enables the FGMultiplay object. */
    bool open();

    /** Tells the multiplayer_mgr to send/receive data.
    */
    bool process();

    /** Closes the multiplayer_mgr.
    */
    bool close();

    // accessors
    void set_hz(const int rate) { _rate = rate; }
    void set_host(const string & host) { _host = host; }
    void set_direction(const int direction) { _direction = direction; }
    void set_port(const int port) { _port = port; }
    void set_enabled(const bool enabled) { _enabled = enabled; }
    void set_suspended(const bool suspended) {_suspended = suspended; }
    void set_sim_time_sec(const double sim_time_sec) { _sim_time_sec = sim_time_sec; }
    void set_Longitude(const double Longitude) { _Longitude = Longitude; }
    void set_Latitude(const double Latitude) { _Latitude = Latitude; }
    void set_Altitude(const double Altitude) { _Altitude = Altitude; }
    void set_Phi(const double Phi) { _Phi = Phi; }
    void set_Theta(const double Phi) { _Phi = Phi; }
    void set_Psi(const double Psi) { _Psi = Psi; }
    void set_U_body(const double U_body) { _U_body = U_body; }
    void set_V_body(const double V_body) { _V_body = V_body; }
    void set_W_body(const double W_body) { _W_body = W_body; }
    void set_P_body(const double P_body) { _P_body = P_body; }
    void set_Q_body(const double Q_body) { _Q_body = Q_body; }
    void set_R_body(const double R_body) { _R_body = R_body; }

    double get_hz() { return _rate; }
    string get_host() { return _host; }
    int get_direction() { return _direction; }
    int get_port() { return _port; }
    bool is_enabled() { return _enabled; }
    bool is_suspended() { return _suspended; }
    double get_sim_time_sec() { return _sim_time_sec; }
    double get_Longitude() { return _Longitude; }
    double get_Latitude() { return _Longitude; }
    double get_Altitude() { return _Altitude; }
    double get_Phi() { return _Phi; }
    double get_Theta() { return _Theta; }
    double get_Psi() { return _Psi; }
    double get_U_body() { return _U_body; }
    double get_V_body() { return _V_body; }
    double get_W_body() { return _W_body; }
    double get_P_body() { return _P_body; }
    double get_Q_body() { return _Q_body; }
    double get_R_body() { return _R_body; }
    FGMultiplayMgr * get_multiplayer_mgr() { return _mgr; }

private:

  void set_multiplayer_mgr(FGMultiplayMgr * mgr) { _mgr = mgr; }

  // Map between the property id's from the multiplayers network packets
  // and the property nodes
  //typedef std::map<unsigned, SGSharedPtr<SGPropertyNode> > PropertyMap;
  //PropertyMap mPropertyMap;
  int _direction;
  int _rate;
  int _port;
  string _host;
  bool _enabled;
  bool _suspended;
  double _sim_time_sec;
  double _Longitude;
  double _Latitude;
  double _Altitude;
  double _Phi;
  double _Theta;
  double _Psi;
  double _U_body;
  double _V_body;
  double _W_body;
  double _P_body;
  double _Q_body;
  double _R_body;
  FGMultiplayMgr * _mgr;
};


#endif // _FG_MULTIPLAY_HXX
