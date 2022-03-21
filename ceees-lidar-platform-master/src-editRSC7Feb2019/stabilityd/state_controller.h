#pragma once

#include <functional>
#include <tuple>

class DaemonStateController {
public:
    enum DaemonState : uint8_t {
        st_init,
        st_idle,
        st_halted,
        st_active,
        st_calibrating,
        st_floating,
        st_deinit
    };

    enum DaemonResponse : uint8_t {
        rp_null,
        rp_error,
        rp_shutdown,
        rp_initialize_gyro,
        rp_initialize_pitch,
        rp_initialize_roll,
        rp_enable_gyro,
        rp_disable_gyro,
        rp_enable_motor_power,
        rp_disable_motor_power,
        rp_release_brake_pitch,
        rp_release_brake_roll,
        rp_set_brake_pitch,
        rp_set_brake_roll,
        rp_enable_pitch,
        rp_enable_roll,
        rp_disable_pitch,
        rp_disable_roll,
        rp_adjust_pitch,
        rp_adjust_roll,
        rp_calibrate
    };

    using DaemonStateTransition = std::tuple<DaemonState, DaemonState>;
    using DaemonStateAttemptedTransition = std::tuple<DaemonStateTransition, bool>;
    using DaemonProgram = std::set<DaemonResponse>;

    explicit DaemonStateController(
        const std::function<bool(DaemonResponse)> &response_handler
    );

    DaemonState current_state();
    bool transition();
    bool transition_to(const DaemonState st_target);

    size_t history_size();
    void history_clear();

    const std::string to_string(const DaemonState &st);
    const std::string to_string(const DaemonResponse &rp);

private:
    const std::map<DaemonStateTransition, DaemonProgram> transition_responses;
    const std::function<bool(DaemonResponse)> &_response_handler;
    void DaemonStateController::_update_state(const DaemonState, bool);
    std::vector<DaemonStateAttemptedTransition> _transition_history;
    size_t _head, _tail;
    DaemonState _st_current;
    std::mutex _update_state;

};

