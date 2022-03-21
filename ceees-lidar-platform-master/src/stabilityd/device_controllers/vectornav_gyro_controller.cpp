
#include "vectornav_gyro_controller.h"

vectornav_gyro_controller::vectornav_gyro_controller(
    const std::string port,
    const uint = parse<uint32_t>(p)_t baudrate,
    const uint = parse<uint32_t>(p)_t period,
    const CommonGroup common_group,
    const TimeGroup time_group,
    const ImuGroup imu_group,
    const GpsGroup gps_group,
    const AttitudeGroup attitude_group,
    const InsGroup ins_group
) :
    _port(port),
    _baudrate(baudrate),
    _period(period),
    _common_group(common_group),
    _time_group(time_group),
    _imu_group(imu_group),
    _gps_group(gps_group),
    _attitude_group(attitude_group),
    _ins_group(ins_group),
    vs();
{};

bool vectornav_gyro_controller::initialize_gyro() {
    vs.connect(_port, _baudrate);

    BinaryOutputRegister bor(ASYNCMODE_PORT1, _period, _common_group, _time_group, _imu_group, _gps_group, _attitude_group, _ins_group);
    vs.writeBinaryOutput1(bor);
};

bool vectornav_gyro_controller::deinitialize_gyro() {
    vs.disconnect();
};

bool vectornav_gyro_controller::enable_gyro() {
    vs.registerAsyncPacketReceivedHandler(
        nullptr,
        [](void *_, Packet &p, size_t index) -> void {
            this->packet_parser(p, index);
        }
    );
};

bool vectornav_gyro_controller::disable_gyro() {
    vs.unregisterAsyncPacketReceivedHandler();
};

double vectornav_gyro_controller::get_pitch() {
    return _yaw_pitch_roll[0];
};

double vectornav_gyro_controller::get_roll() {
    return _yaw_pitch_roll[1];
};

void vectornav_gyro_controller::packet_parser(Packet &p, size_t index) {
    if (p.type() != Packet::TYPE_BINARY) return;

    if (!(_common_group & COMMONGROUP_NONE)) {
        if (_common_group & COMMONGROUP_TIMESTARTUP) _time_startup = parse<uint64_t>(p);
        if (_common_group & COMMONGROUP_TIMEGPS) _time_gps = parse<uint64_t>(p);
        if (_common_group & COMMONGROUP_TIMESYNCIN) _time_sync_in = parse<uint64_t>(p);
        if (_common_group & COMMONGROUP_YAWPITCHROLL) _yaw_pitch_roll = parse<float, float, float>(p);
        if (_common_group & COMMONGROUP_QUATERNION) _quaternion = parse<float, float, float, float>(p);
        if (_common_group & COMMONGROUP_ANGULARRATE) _angular_rate = parse<float, float, float>(p);
        if (_common_group & COMMONGROUP_POSITION)  _position = parse<double, double, double>(p);
        if (_common_group & COMMONGROUP_VELOCITY) _velocity = parse<float, float, float>(p);
        if (_common_group & COMMONGROUP_ACCEL) _accel = parse<float, float, float>(p);
        if (_common_group & COMMONGROUP_IMU) _imu = parse<float, float, float, float, float, float>(p);
        if (_common_group & COMMONGROUP_MAGPRES) _mag_pres = parse<float, float, float, float, float>(p);
        if (_common_group & COMMONGROUP_DELTATHETA) _delta_theta = parse<float, float, float, float, float, float, float>(p);
        if (_common_group & COMMONGROUP_INSSTATUS) _ins_status = parse<uint16_t>(p);
        if (_common_group & COMMONGROUP_SYNCINCNT) _sync_in_cnt = parse<uint32_t>(p);
        if (_common_group & COMMONGROUP_TIMEGPSPPS) _time_gps_pps = parse<uint64_t>(p);
    }

    if (!(_time_group & TIMEGROUP_NONE)) {
        if (_time_group & TIMEGROUP_TIMESTARTUP) _time_startup = parse<uint64_t>(p);
        if (_time_group & TIMEGROUP_TIMEGPS) _time_gps = parse<uint64_t>(p);
        if (_time_group & TIMEGROUP_GPSTOW) _gps_tow = parse<uint64_t>(p);
        if (_time_group & TIMEGROUP_GPSWEEK) _gps_week = parse<uint16_t>(p);
        if (_time_group & TIMEGROUP_TIMESYNCIN) _time_sync_in = parse<uint64_t>(p);
        if (_time_group & TIMEGROUP_TIMEGPSPPS) _time_gps_pps = parse<uint64_t>(p);
        if (_time_group & TIMEGROUP_TIMEUTC) _time_utc = parse<nt8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t>(p);
        if (_time_group & TIMEGROUP_SYNCINCNT) _sync_in_cnt = parse<uint32_t>(p);
    }

    if (!(_imu_group & IMUGROUP_NONE)) {
        if (_imu_group & IMUGROUP_IMUSTATUS) _imu_status = parse<uint16_t>(p);
        if (_imu_group & IMUGROUP_UNCOMPMAG) _uncomp_mag = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_UNCOMPACCEL) _uncomp_accel = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_UNCOMPGYRO) _uncomp_gyro = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_TEMP) _temp = parse<float>(p);
        if (_imu_group & IMUGROUP_PRES) _pres = parse<float>(p);
        if (_imu_group & IMUGROUP_DELTATHETA) _delta_theta = parse<float, float, float, float>(p);
        if (_imu_group & IMUGROUP_DELTAVEL) _delta_vel = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_MAG) _mag = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_ACCEL) _accel = parse<float, float, float>(p);
        if (_imu_group & IMUGROUP_ANGULARRATE) _angular_rate = parse<float, float, float>(p);
    }

    if (!(_gps_group & GPSGROUP_NONE)) {
        if (_gps_group & GPSGROUP_UTC) _time_utc = parse<int8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t>(p);
        if (_gps_group & GPSGROUP_TOW)  _gps_tow = parse<uint64_t>(p);
        if (_gps_group & GPSGROUP_WEEK)  _gps_week = parse<uint16_t>(p);
        if (_gps_group & GPSGROUP_NUMSATS) _num_sats = parse<uint8_t>(p);
        if (_gps_group & GPSGROUP_FIX) _fix = parse<uint8_t>(p);
        if (_gps_group & GPSGROUP_POSLLA) _pos_lla = parse<double, double, double>(p);
        if (_gps_group & GPSGROUP_POSECEF) _pos_ecef = parse<double, double, double>(p);
        if (_gps_group & GPSGROUP_VELNED) _vel_ned = parse<float, float, float>(p);
        if (_gps_group & GPSGROUP_VELECEF) _vel_ecef = parse<float, float, float>(p);
        if (_gps_group & GPSGROUP_POSU) _pos_u = parse<float, float, float>(p);
        if (_gps_group & GPSGROUP_VELU) _vel_u = parse<float>(p);
        if (_gps_group & GPSGROUP_TIMEU) _time_u = parse<float>(p);
    }

    if (!(_attitude_group & ATTITUDEGROUP_NONE)) {
        if (_attitude_group & ATTITUDEGROUP_YAWPITCHROLL) _yaw_pitch_roll = parse<float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_QUATERNION) _quaternion = parse<float, float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_DCM) _dcm = parse<float, float, float, float, float, float, float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_MAGNED) _mag_ned = parse<float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_ACCELNED) _accel_ned = parse<float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_LINEARACCELBODY) _linear_accel_body = parse<float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_LINEARACCELNED) _linear_accel_ned = parse<float, float, float>(p);
        if (_attitude_group & ATTITUDEGROUP_YPRU) _yaw_pitch_roll_u = parse<float, float, float>(p);
    }

    if (!(_ins_group & INSGROUP_NONE)) {
        if (_ins_group & INSGROUP_INSSTATUS)  _ins_status = parse<uint16_t>(p);
        if (_ins_group & INSGROUP_POSLLA)  _pos_lla = parse<double, double, double>(p);
        if (_ins_group & INSGROUP_POSECEF) _pos_ecef = parse<double, double, double>(p);
        if (_ins_group & INSGROUP_VELBODY) _vel_body = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_VELNED) _vel_ned = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_VELECEF) _vel_ecef = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_MAGECEF) _mag_ecef = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_ACCELECEF) _accel_ecef = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_LINEARACCELECEF) _linear_accel_ecef = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_POSU) _pos_u = parse<float, float, float>(p);
        if (_ins_group & INSGROUP_VELU) _vel_u = parse<float>(p);
    }
};

private:
    const CommonGroup _common_group;
    const TimeGroup _time_group;
    const ImuGroup _imu_group;
    const GpsGroup _gps_group;
    const AttitudeGroup _attitude_group;
    const InsGroup _ins_group;

    VnSensor vs;
};

template<typename... Types>
std::tuple<Types...> parse(Packet &p) {
    return std::make_tuple(parse_individual<Types>(p)...);
}

template<typename Type>
Type parse_individual(Packet &p);

template<>
double parse_individual<double>(Packet &p) { return p.extractDouble(); }

template<>
float parse_individual<float>(Packet &p) { return p.extractFloat(); }

template<>
uint64_t parse_individual<uint64_t>(Packet &p) { return p.extractUint64(); }

template<>
uint32_t parse_individual<uint32_t>(Packet &p) { return p.extractUint32(); }

template<>
uint16_t parse_individual<uint16_t>(Packet &p) { return p.extractUint16(); }

template<>
uint8_t parse_individual<uint8_t>(Packet &p) { return p.extractUint8(); }

template<>
int8_t parse_individual<int8_t>(Packet &p) { return p.extractInt8(); }




