#include <map>
#include <set>
#include <mutex>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <unistd.h>
#include <csignal>

#include "device_controllers/generic_device_controller.h"
#include "state_controller.h"
#include "signal_adapter.h"
#include "daemonize.h"

// Controller defined by states
// Responses are events that depend on (last_state, current_state) tuple

int main(int argc, char *argv[]) {

    auto audit_log = spdlog::rotating_logger_mt("CasperX Controller", "/tmp/casperx_log.txt", 1048576, 3);
    audit_log->flush_on(spdlog::level::info);

    DaemonStateController sc([](DaemonResponse rp) -> bool {
        switch (rp) {
            default: return false;
            case rp_null: return true;
            case rp_error: /* log error; */ return false;
            case rp_initialize_gyro: return device_controller.initialize_gyro();
            case rp_initialize_pitch: return device_controller.initialize_pitch();
            case rp_initialize_roll: return device_controller.initialize_roll();
            case rp_enable_gyro: return device_controller.enable_gyro();
            case rp_disable_gyro: return device_controller.disable_gyro();
            case rp_enable_motor_power: return device_controller.enable_motor_power();
            case rp_disable_motor_power: return device_controller.disable_motor_power();
            case rp_release_brake_pitch: return device_controller.release_brake_pitch();
            case rp_release_brake_roll: return device_controller.release_brake_roll();
            case rp_set_brake_pitch: return device_controller.set_brake_roll();
            case rp_set_brake_roll: return device_controller.set_brake_roll();
            case rp_enable_pitch: return device_controller.enable_pitch();
            case rp_enable_roll: return device_controller.enable_roll();
            case rp_disable_pitch: return device_controller.disable_pitch();
            case rp_disable_roll: return device_controller.disable_roll();
            case rp_calibrate: return device_controller.calibrate();
            case rp_adjust_pitch:
                return device_controller.adjust_pitch();
                break;
            case rp_adjust_roll:
                return device_controller.adjust_roll();
                break;
            case rp_shutdown:
                device_controller.shutdown();
                break;
        }
    });

    // SIGINFO -> INFO DUMP
    // SIGRTMIN -> RE-FLASH PITCH CONTROLLER
    // SIGRTMAX -> RE-FLASH ROLL CONTROLLER

    return daemonize("CasperX Controller", [&sc]() {
        sc.transition();

        switch (sc.current_state()) {
            case st_init: return daemon_wait(1000000);
            case st_idle: return daemon_wait(500000);
            case st_halted: return daemon_wait(500000);
            case st_active: return daemon_wait(1000);
            case st_calibrating: return daemon_wait(1000);
            case st_floating: return daemon_wait(1000);
            case st_deinit: return daemon_exit(EXIT_SUCCESS);
            default: return daemon_exit(EXIT_FAILURE);
        }
    });
}