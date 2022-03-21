#pragma once

#include "generic_motor_controller.h"
#include "generic_gyro_controller.h"

class generic_device_controller : public generic_motor_controller, public generic_gyro_controller {
public:
    virtual bool calibrate()=0;
}
