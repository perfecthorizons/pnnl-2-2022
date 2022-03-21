#pragma once

class generic_motor_controller {
public:
    virtual bool initialize_pitch()=0;
    virtual bool initialize_roll()=0;
    virtual bool enable_motor_power()=0;
    virtual bool disable_motor_power()=0;
    virtual bool release_brake_pitch()=0;
    virtual bool release_brake_roll()=0;
    virtual bool set_brake_roll()=0;
    virtual bool set_brake_roll()=0;
    virtual bool enable_pitch()=0;
    virtual bool enable_roll()=0;
    virtual bool disable_pitch()=0;
    virtual bool disable_roll()=0;
    virtual bool adjust_pitch()=0;
    virtual bool adjust_roll()=0;
//    virtual bool adjust_pitch_velocity()=0;
//    virtual bool adjust_roll_velocity()=0;
    
protected:
    virtual double get_pitch()=0;
    virtual double get_roll()=0;
};
