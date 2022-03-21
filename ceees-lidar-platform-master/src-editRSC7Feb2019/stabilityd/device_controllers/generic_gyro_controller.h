#pragma once

class generic_gyro_controller {
public:
    virtual bool initialize_gyro()=0;
    virtual bool deinitialize_gyro()=0;
    virtual bool enable_gyro()=0;
    virtual bool disable_gyro()=0;
protected:
    virtual double get_pitch()=0;
    virtual double get_roll()=0;
};
