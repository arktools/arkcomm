#include "multiplaymgr.hxx"
#include <iostream>
#include <vector>

int main (int argc, char const* argv[])
{
    std::vector<FGPropertyData*> properties;
    FGExternalMotionData data;
    data.angularVel = SGVec3f(0,0,0);
    data.angularVel = SGVec3f(0,0,0);
    data.lag = 0;
    data.linearAccel = SGVec3f(0,0,0);
    data.linearVel = SGVec3f(0,0,0);
    data.orientation = SGQuatf(1,0,0,0);
    data.position = SGVec3d(0,0,0);
    data.properties = properties;
    data.time = 0;

    FGMultiplayMgr manager;
    manager.init();
    while(1) {
        manager.Update();
        std::cout << "updating" << std::endl;
        usleep(100000);
        manager.SendTextMessage("hello"); 
        manager.SendMyPosition(data);
    }
    return 0;
}
