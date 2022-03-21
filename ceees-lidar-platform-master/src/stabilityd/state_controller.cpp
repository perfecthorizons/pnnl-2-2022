#include "state_controller.h"

DaemonStateController::DaemonStateController(
    const std::function<bool(DaemonResponse)> &response_handler
) :
    _st_current(st_init),
    _response_handler(response_handler),
    _transition_history(),
    _head(0),
    _tail(0),
    _transition_responses({
        {{st_init, st_init}, {rp_error}}
        {{st_init, st_calibrating}, {rp_error}}
        {{st_init, st_deinit}, {rp_error}}
        {{st_init, st_floating}, {rp_error}}
        {{st_init, st_halted}, {rp_error}}
        {{st_init, st_active}, {rp_error}}
        {{st_init, st_idle}, {rp_initialize_gyro, rp_enable_motor_power, rp_initialize_pitch, rp_initialize_roll}}
        {{st_calibrating, st_init}, {rp_error}}
        {{st_calibrating, st_calibrating}, {rp_error}}
        {{st_calibrating, st_deinit}, {rp_error}}
        {{st_calibrating, st_floating}, {rp_error}}
        {{st_calibrating, st_halted}, {rp_disable_motor_power}}
        {{st_calibrating, st_active}, {rp_error}}
        {{st_calibrating, st_idle}, {rp_null}}
        {{st_deinit, st_init}, {rp_error}}
        {{st_deinit, st_calibrating}, {rp_error}}
        {{st_deinit, st_deinit}, {rp_error}}
        {{st_deinit, st_floating}, {rp_error}}
        {{st_deinit, st_halted}, {rp_error}}
        {{st_deinit, st_active}, {rp_error}}
        {{st_deinit, st_idle}, {rp_error}}
        {{st_floating, st_init}, {rp_error}}
        {{st_floating, st_calibrating}, {rp_error}}
        {{st_floating, st_deinit}, {rp_error}}
        {{st_floating, st_floating}, {rp_error}}
        {{st_floating, st_halted}, {rp_disable_motor_power}}
        {{st_floating, st_active}, {rp_error}}
        {{st_floating, st_idle}, {rp_null}}
        {{st_halted, st_init}, {rp_error}}
        {{st_halted, st_calibrating}, {rp_error}}
        {{st_halted, st_deinit}, {rp_error}}
        {{st_halted, st_floating}, {rp_error}}
        {{st_halted, st_halted}, {rp_null}}
        {{st_halted, st_active}, {rp_error}}
        {{st_halted, st_idle}, {rp_enable_motor_power, rp_initialize_pitch, rp_initialize_roll, rp_disable_gyro}}
        {{st_active, st_init}, {rp_error}}
        {{st_active, st_calibrating}, {rp_error}}
        {{st_active, st_deinit}, {rp_error}}
        {{st_active, st_floating}, {rp_error}}
        {{st_active, st_halted}, {rp_disable_motor_power}}
        {{st_active, st_active}, {rp_adjust_pitch, rp_adjust_roll}}
        {{st_active, st_idle}, {rp_disable_pitch, rp_disable_roll, rp_disable_gyro}}
        {{st_idle, st_init}, {rp_error}}
        {{st_idle, st_calibrating}, {rp_calibrate}}
        {{st_idle, st_deinit}, {rp_disable_pitch, rp_disiable_roll, rp_disable_motor_power, rp_shutdown}}
        {{st_idle, st_floating}, {rp_release_brake_pitch, rp_release_brake_roll}}
        {{st_idle, st_halted}, {rp_disable_motor_power, rp_enable_gyro}}
        {{st_idle, st_active}, {rp_enable_gyro, rp_enable_pitch, rp_enable_roll}}
        {{st_idle, st_idle}, {rp_null}}
    })
{};

void DaemonStateController::_update_state(const DaemonState st_target, bool success) {
    if (st_target != _st_current) {
        _transition_history.emplace_back(std::make_tuple(_st_current, st_target), success);
        if (success) _st_current = st_target;
    }
}

DaemonState DaemonStateController::current_state() {
    return _st_current;
}

bool DaemonStateController::transition() {
    return transition_to(_st_current);
}

bool DaemonStateController::transition_to(const DaemonState st_target) {
    std::lock_guard<std::mutex> lock(_update_state);

    bool transition_success = _response_handler(transition_responses[std::make_pair(_st_current, st_target)]);
    _update_state(st_target, transitition_succes);

    return transition_success;
}

size_t DaemonStateController::history_size() {
    return _transition_history.size();
};

void history_clear() {
    _transition_history.clear();
};

const std::string DaemonStateController::to_string(const DaemonState &st) {
    switch (st) {
        case st_null: return "st_null";
        case st_init: return "st_init";
        case st_idle: return "st_idle";
        case st_halted: return "st_halted";
        case st_active: return "st_active";
        case st_calibrating: return "st_calibrating";
        case st_floating: return "st_floating";
        case st_deinit: return "st_deinit";
        default: return "st_unknown";
    }
};

const std::string DaemonStateController::to_string(const DaemonResponse &rp) {
    switch (rp) {
        case rp_error: return "rp_error";
        case rp_shutdown: return "rp_shutdown";
        case rp_initialize_gyro: return "rp_initialize_gyro";
        case rp_initialize_pitch: return "rp_initialize_pitch";
        case rp_initialize_roll: return "rp_initialiez_roll";
        case rp_enable_gyro: return "rp_enable_gyro";
        case rp_disable_gyro: return "rp_disable_gyro";
        case rp_enable_motor_power: return "rp_enable_motor_power";
        case rp_disable_motor_power: return "rp_disable_motor_power";
        case rp_release_brake_pitch: return "rp_release_brake_pitch";
        case rp_release_brake_roll: return "rp_release_brake_roll";
        case rp_set_brake_pitch: return "rp_set_brake_pitch";
        case rp_set_brake_roll: return "rp_set_brake_roll";
        case rp_enable_pitch: return "rp_enable_pitch";
        case rp_enable_roll: return "rp_enable_roll";
        case rp_disable_pitch: return "rp_disable_pitch";
        case rp_disable_roll: return "rp_disable_roll";
        case rp_adjust_pitch: return "rp_adjust_pitch";
        case rp_adjust_roll: return "rp_adjust_roll";
        case rp_calibrate: return "rp_calibrate";
        default: : return "rp_unknown";
    }
};

