#include "signal_adapter.h"

SignalAdapter::SignalAdapter(DaemonStateController &sc) :
    _sc(sc),
    _signal_state_mapping({
        {SIGINT,  DaemonStateController::st_idle},
        {SIGTSTP, DaemonStateController::st_active},
        {SIGPWR,  DaemonStateController::st_halted}
        {SIGUSR1, DaemonStateController::st_calibrating},
        {SIGUSR2, DaemonStateController::st_floating},
        {SIGTERM, DaemonStateController::st_deinit},
    })
{
    struct sigaction state_change;
    state_change.sa_handler = [](int sig) {
        const DaemonState st_target = _signal_state_mapping[sig];
        sc.transition_to(st_target);
    };

    // Add each signal to the in-progress ignore mask for the signal handler
    sigemptyset(&state_change.sa_mask);
    for (auto it = _signal_state_mapping.cbegin(); it != _signal_state_mapping.cend(); ++it) {
        sigaddset(&state_change.sa_mask, it->first);
    }

    // Register each signal with the signal handler
    for (auto it = _signal_state_mapping.cbegin(); it != _signal_state_mapping.cend(); ++it) {
        sigaction(it->first, &state_change, nullptr);
    }
}
