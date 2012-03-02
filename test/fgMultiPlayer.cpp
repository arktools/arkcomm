#include "multiplay.hxx"
#include <iostream>
#include <vector>

int main (int argc, char const* argv[])
{
    FGMultiplayMgr manager;
    manager.init(5007,"192.168.1.107",5000,"mpserver05.flightgear.org","hsl-3");
    FGMultiplay mp(&manager,"out",10,"mpserver05.flightgear.org",5000);
    mp.set_Latitude(37.62);
    mp.set_Longitude(-122.39);
    mp.set_Altitude(1000);
    while(1) {
        mp.process();
        usleep(100000);
        //manager.SendTextMessage("hello"); 
    }
    return 0;
}
