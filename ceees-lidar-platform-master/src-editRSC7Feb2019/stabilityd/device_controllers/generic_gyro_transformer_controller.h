#pragma once

#include "generic_gyro_controller.h"

template<double circle_units = 360.0>
class generic_gyro_transfomer_controller : public generic_gyro_controller {
    double get_pitch() {
        
    };
    double get_roll() {
    };
    double get_pitch_roll() {
        float [pitch, roll] = generic_gyro_controller::get_pitch_roll();

        // scale
        pitch *= scale_pitch;
        roll *= scale_roll;
        
        // skew
        pitch = (pitch + skew_pitch) % circle_units;
        roll = (roll + skew_roll) % circle_units;

        // reflect
        if (reflect_pitch) pitch = (-pitch % circle_units);
        if (reflect_roll) roll = (-roll % circle_units);

        // rotate
        float _pitch = pitch, _roll = roll;
        pitch = _pitch * cos(rotation) - _roll * sin(rotation);
        roll = _pitch * sin(rotation) + _roll * cos(rotation);
        
        // clamp
        pitch = max(min(pitch, clamp_pitch[1]), clamp_pitch[0]);
        roll = max(min(roll, clamp_roll[1]), clamp_roll[0]);
        
    }
protected:
    bool reflect_pitch, reflection_roll;
    double scale_pitch, scale_roll;
    double rotation;
    double skew_pitch, skew_roll;
    std::tuple<double, double> clamp_pitch, clamp_roll;
}
