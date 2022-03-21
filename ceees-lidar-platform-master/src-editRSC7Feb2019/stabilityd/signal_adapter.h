#pragma once

#include "state_controller.h"

class SignalAdapter {
public:
    SignalAdapter(DaemonStateController &sc);
private:
    const std::map<const int, const DaemonStateController::DaemonState> _signal_state_mapping;
    DaemonStateController &_sc;
};