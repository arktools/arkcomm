// multiplay.cxx -- protocol object for multiplay in Flightgear
//
// Written by Diarmuid Tyson, started February 2003.
// diarmuid.tyson@airservicesaustralia.com
//
// With addtions by Vivian Meazza, January 2006
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <simgear/compiler.h>
#include <simgear/io/iochannel.hxx>

#include <string>

#include <iostream>
#include <map>
#include <string>

#include <simgear/debug/logstream.hxx>
#include <simgear/math/SGMath.hxx>

//#include <FDM/flight.hxx>
#include <MultiPlayer/mpmessages.hxx>

#include "multiplay.hxx"

using std::string;


// These constants are provided so that the ident command can list file versions.
const char sFG_MULTIPLAY_BID[] = "$Id$";
const char sFG_MULTIPLAY_HID[] = FG_MULTIPLAY_HID;


/******************************************************************
* Name: FGMultiplay
* Description: Constructor.  Initialises the protocol and stores
* host and port information.
******************************************************************/
FGMultiplay::FGMultiplay (FGMultiplayMgr * mgr, const string dir, const int rate, const string &host, const int port) :
    _direction(SG_IO_NONE),
    _rate(rate),
    _port(port),
    _host(host),
    _enabled(false),
    _suspended(false),
    _sim_time_sec(0),
    _Longitude(0),
    _Latitude(0),
    _Altitude(0),
    _Phi(0),
    _Theta(0),
    _Psi(0),
    _U_body(0),
    _V_body(0),
    _W_body(0),
    _P_body(0),
    _Q_body(0),
    _R_body(0),
    _mgr(mgr)
{
    if (dir == "in") {
        _direction = SG_IO_IN;
    }
    else if (dir == "out") {
        _direction = SG_IO_OUT;
    }
}


/******************************************************************
* Name: ~FGMultiplay
* Description: Destructor.
******************************************************************/
FGMultiplay::~FGMultiplay () {
}


/******************************************************************
* Name: open
* Description: Enables the protocol.
******************************************************************/
bool FGMultiplay::open() {

    if ( is_enabled() ) {
	SG_LOG( SG_IO, SG_ALERT, "This shouldn't happen, but the channel "
		<< "is already in use, ignoring" );
	return false;
    }

    set_enabled(true);

    /// Build up the id to property map
    
    //for (unsigned i = 0; i < FGMultiplayMgr::numProperties; ++i) {
      //const char* name = FGMultiplayMgr::sIdPropertyList[i].name;
      //SGPropertyNode* pNode = root->getNode(name);
      //if (pNode)
        //mPropertyMap[FGMultiplayMgr::sIdPropertyList[i].id] = pNode;
    //}

    return is_enabled();
}


/******************************************************************
* Name: process
* Description: Prompts the multiplayer mgr to either send
* or receive data over the network
******************************************************************/
bool FGMultiplay::process() {

  get_multiplayer_mgr()->Update();

  if (get_direction() == SG_IO_OUT) {

    // check if we have left initialization phase. That will not provide
    // interresting data, also the freeze in simulation time hurts the
    // multiplayer clients
    double sim_time = get_sim_time_sec();
//     if (sim_time < 20)
//       return true;

    FGMultiplay *ifce = this;

    // put together a motion info struct, you will get that later
    // from FGInterface directly ...
    FGExternalMotionData motionInfo;

    // The current simulation time we need to update for,
    // note that the simulation time is updated before calling all the
    // update methods. Thus it contains the time intervals *end* time.
    // The FDM is already run, so the states belong to that time.
    motionInfo.time = sim_time;

    // The typical lag will be the reciprocal of the output frequency
    double hz = get_hz();
    if (hz != 0) // I guess we can test a double for exact zero in this case
      motionInfo.lag = 1/get_hz();
    else
      motionInfo.lag = 0.1; //??

    // These are for now converted from lat/lon/alt and euler angles.
    // But this should change in FGInterface ...
    double lon = ifce->get_Longitude();
    double lat = ifce->get_Latitude();
    // first the aprioriate structure for the geodetic one
    SGGeod geod = SGGeod::fromRadFt(lon, lat, ifce->get_Altitude());
    // Convert to cartesion coordinate
    motionInfo.position = SGVec3d::fromGeod(geod);
    
    // The quaternion rotating from the earth centered frame to the
    // horizontal local frame
    SGQuatf qEc2Hl = SGQuatf::fromLonLatRad((float)lon, (float)lat);
    // The orientation wrt the horizontal local frame
    float heading = ifce->get_Psi();
    float pitch = ifce->get_Theta();
    float roll = ifce->get_Phi();
    SGQuatf hlOr = SGQuatf::fromYawPitchRoll(heading, pitch, roll);
    // The orientation of the vehicle wrt the earth centered frame
    motionInfo.orientation = qEc2Hl*hlOr;

    if (is_suspended()) {
      // velocities
      motionInfo.linearVel = SG_FEET_TO_METER*SGVec3f(ifce->get_U_body(),
                                                      ifce->get_V_body(),
                                                      ifce->get_W_body());
      motionInfo.angularVel = SGVec3f(ifce->get_P_body(),
                                      ifce->get_Q_body(),
                                      ifce->get_R_body());
      
      // accels, set that to zero for now.
      // Angular accelerations are missing from the interface anyway,
      // linear accelerations are screwed up at least for JSBSim.
//  motionInfo.linearAccel = SG_FEET_TO_METER*SGVec3f(ifce->get_U_dot_body(),
//                                                    ifce->get_V_dot_body(),
//                                                    ifce->get_W_dot_body());
      motionInfo.linearAccel = SGVec3f::zeros();
      motionInfo.angularAccel = SGVec3f::zeros();
    } else {
      // if the interface is suspendend, prevent the client from
      // wild extrapolations
      motionInfo.linearVel = SGVec3f::zeros();
      motionInfo.angularVel = SGVec3f::zeros();
      motionInfo.linearAccel = SGVec3f::zeros();
      motionInfo.angularAccel = SGVec3f::zeros();
    }

    // now send the properties
    //PropertyMap::iterator it;
    //for (it = mPropertyMap.begin(); it != mPropertyMap.end(); ++it) {
      //FGPropertyData* pData = new FGPropertyData;
      //pData->id = it->first;
      //pData->type = it->second->getType();
      
      //switch (pData->type) {
        //case SGPropertyNode::INT:        
        //case SGPropertyNode::LONG:       
        //case SGPropertyNode::BOOL:
          //pData->int_value = it->second->getIntValue();
          //break;
        //case SGPropertyNode::FLOAT:
        //case SGPropertyNode::DOUBLE:
          //pData->float_value = it->second->getFloatValue();
          //break;
        //case SGPropertyNode::STRING:
        //case SGPropertyNode::UNSPECIFIED:
          //{
            //// FIXME: We assume unspecified are strings for the moment.

            //const char* cstr = it->second->getStringValue();
            //int len = strlen(cstr);
            
            //if (len > 0)
            //{            
              //pData->string_value = new char[len + 1];
              //strcpy(pData->string_value, cstr);
            //}
            //else
            //{
              //// Size 0 - ignore
              //pData->string_value = 0;            
            //}

            ////cout << " Sending property " << pData->id << " " << pData->type << " " <<  pData->string_value << "\n";
            //break;        
          //}
        //default:
          //// FIXME Currently default to a float. 
          ////cout << "Unknown type when iterating through props: " << pData->type << "\n";
          //pData->float_value = it->second->getFloatValue();
          //break;
      //}
      
      //motionInfo.properties.push_back(pData);
    //}

    FGMultiplayMgr* mpmgr = get_multiplayer_mgr();
    mpmgr->SendMyPosition(motionInfo);
    
    // Now remove the data
    //std::vector<FGPropertyData*>::const_iterator propIt;
    //std::vector<FGPropertyData*>::const_iterator propItEnd;
    //propIt = motionInfo.properties.begin();
    //propItEnd = motionInfo.properties.end();

    //cout << "Deleting data\n";

    //while (propIt != propItEnd)
    //{
      //delete *propIt;
      //propIt++;
    //}    
  }

  return true;
}


/******************************************************************
* Name: close
* Description:  Closes the multiplayer mgrs to stop any further
* network processing
******************************************************************/
bool FGMultiplay::close() {

  FGMultiplayMgr *mgr = get_multiplayer_mgr();

  if (mgr == 0) {
    return false;
  }

  if (get_direction() == SG_IO_IN) {

    mgr->Close();

  } else if (get_direction() == SG_IO_OUT) {

    mgr->Close();

  }

  return true;
}

