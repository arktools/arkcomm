#include "flightGearUtils.hpp"

void htond (double &x)
{
    if ( sgIsLittleEndian() )
    {
        int    *Double_Overlay;
        int     Holding_Buffer;

        Double_Overlay = (int *) &x;
        Holding_Buffer = Double_Overlay [0];

        Double_Overlay [0] = htonl (Double_Overlay [1]);
        Double_Overlay [1] = htonl (Holding_Buffer);
    }
    else
    {
        return;
    }
}

void htonf (float &x)
{
    if ( sgIsLittleEndian() )
    {
        int    *Float_Overlay;
        int     Holding_Buffer;

        Float_Overlay = (int *) &x;
        Holding_Buffer = Float_Overlay [0];

        Float_Overlay [0] = htonl (Holding_Buffer);
    }
    else
    {
        return;
    }
}

void htonCtrls( FGNetCtrls & net)
{
    // convert to network byte order
    net.version = htonl(net.version);
    htond(net.aileron);
    htond(net.elevator);
    htond(net.rudder);
    htond(net.aileron_trim);
    htond(net.elevator_trim);
    htond(net.rudder_trim);
    htond(net.flaps);
    net.flaps_power = htonl(net.flaps_power);
    net.flap_motor_ok = htonl(net.flap_motor_ok);

    net.num_engines = htonl(net.num_engines);
    for (int i = 0; i < FGNetCtrls::FG_MAX_ENGINES; ++i )
    {
        net.master_bat[i] = htonl(net.master_bat[i]);
        net.master_alt[i] = htonl(net.master_alt[i]);
        net.magnetos[i] = htonl(net.magnetos[i]);
        net.starter_power[i] = htonl(net.starter_power[i]);
        htond(net.throttle[i]);
        htond(net.mixture[i]);
        net.fuel_pump_power[i] = htonl(net.fuel_pump_power[i]);
        htond(net.prop_advance[i]);
        htond(net.condition[i]);
        net.engine_ok[i] = htonl(net.engine_ok[i]);
        net.mag_left_ok[i] = htonl(net.mag_left_ok[i]);
        net.mag_right_ok[i] = htonl(net.mag_right_ok[i]);
        net.spark_plugs_ok[i] = htonl(net.spark_plugs_ok[i]);
        net.oil_press_status[i] = htonl(net.oil_press_status[i]);
        net.fuel_pump_ok[i] = htonl(net.fuel_pump_ok[i]);
    }

    net.num_tanks = htonl(net.num_tanks);
    for (int i = 0; i < FGNetCtrls::FG_MAX_TANKS; ++i )
    {
        net.fuel_selector[i] = htonl(net.fuel_selector[i]);
    }

    net.cross_feed = htonl(net.cross_feed);
    htond(net.brake_left);
    htond(net.brake_right);
    htond(net.copilot_brake_left);
    htond(net.copilot_brake_right);
    htond(net.brake_parking);
    net.gear_handle = htonl(net.gear_handle);
    net.master_avionics = htonl(net.master_avionics);
    htond(net.wind_speed_kt);
    htond(net.wind_dir_deg);
    htond(net.turbulence_norm);
    htond(net.temp_c);
    htond(net.press_inhg);
    htond(net.hground);
    htond(net.magvar);
    net.icing = htonl(net.icing);
    net.speedup = htonl(net.speedup);
    net.freeze = htonl(net.freeze);
}


void htonFdm( FGNetFDM & net )
{
    net.version = ntohl(net.version);

    htond(net.latitude);
    htond(net.longitude);
    htond(net.altitude);

    htonf(net.phi);
    htonf(net.theta);
    htonf(net.psi);

    htonf(net.phidot);
    htonf(net.thetadot);
    htonf(net.psidot);

    htonf(net.vcas);

    htonf(net.A_X_pilot);
    htonf(net.A_Y_pilot);
    htonf(net.A_Z_pilot);

    htonf(net.alpha);
    htonf(net.beta);
}

