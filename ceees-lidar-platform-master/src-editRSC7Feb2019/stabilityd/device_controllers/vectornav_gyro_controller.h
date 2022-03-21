#pragma once

#include "generic_gyro_controller.h"

class vectornav_gyro_controller : public generic_gyro_controller {
public:
    vectornav_gyro_controller(
        const std::string port,
        const uint32_t baudrate,
        const CommonGroup common_group,
        const TimeGroup time_group,
        const ImuGroup imu_group,
        const GpsGroup gps_group,
        const AttitudeGroup attitude_group,
        const InsGroup ins_group
    );

    bool initialize_gyro();
    bool deinitialize_gyro();
    bool enable_gyro();
    bool disable_gyro();

    double get_pitch();
    double get_roll();

private:
    const std::string _port;
    const uint32_t _baudrate;
    const uint32_t _period;

    const CommonGroup _common_group;
    const TimeGroup _time_group;
    const ImuGroup _imu_group;
    const GpsGroup _gps_group;
    const AttitudeGroup _attitude_group;
    const InsGroup _ins_group;

    VnSensor _vs;

    // TODO: unpack tuples

    uint64_t _time_startup;
    uint64_t _time_gps;
    uint64_t _time_sync_in;
    std::tuple<float, float, float> _yaw_pitch_roll;
    std::tuple<float, float, float, float> _quaternion;
    std::tuple<float, float, float> _angular_rate;
    std::tuple<double, double, double> _position;
    std::tuple<float, float, float> _velocity;
    std::tuple<float, float, float> _accel;
    std::tuple<float, float, float, float, float, float> _imu;
    std::tuple<float, float, float, float, float> _mag_pres;
    std::tuple<float, float, float, float, float, float, float> _delta_theta;
    uint16_t _ins_status;
    uint32_t _sync_in_count;
    uint64_t _time_gps_ps;

    uint64_t _gps_tow;
    uint16_t _gps_week;
    uint8_t _num_sats;
    uint8_t _fix;
    std::tuple<double, double, double> _pos_lla;
    std::tuple<double, double, double> _pos_ecef;
    std::tuple<float, float, float> _vel_ned;
    std::tuple<float, float, float> _vel_ecef;
    std::tuple<float, float, float> _vel_body;
    std::tuple<float, float, float> _mag_ecef;
    std::tuple<float, float, float> _accel_ecef;
    std::tuple<float, float, float> _linear_accel_ecef;
    std::tuple<float, float, float> _pos_u;
    float _vel_u;
    float _time_u;
    struct time_utc _time_utc;

    uint16_t _imu_status;
    std::tuple<float, float, float> _uncomp_mag;
    std::tuple<float, float, float> _uncomp_accel;
    std::tuple<float, float, float> _uncomp_gyro;
    float _temp;
    float _pressure;
    std::tuple<float, float, float, float> _delta_theta;
    std::tuple<float, float, float> _delta_vel;
    std::tuple<float, float, float> _mag;

    std::tuple<float, float, float, float, float, float, float, float, float> _dcm;

    std::tuple<float, float, float> _mag_ned;
    std::tuple<float, float, float> _accel_ned;
    std::tuple<float, float, float> _linear_accel_body;
    std::tuple<float, float, float> _linear_accel_ned;
    std::tuple<float, float, float> _yaw_pitch_roll_u;

public:
    void packet_parser(Packet &, size_t);
};
