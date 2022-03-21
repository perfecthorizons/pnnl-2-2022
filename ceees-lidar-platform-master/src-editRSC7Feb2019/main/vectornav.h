#ifndef _VECTORNAV_PACKET_T
#define _VECTORNAV_PACKET_T

#include "shared_memory.h"
#include "vn/sensors.h"
#include "dynamic_functions.h"


#define VECTORNAV_OUTPUT_FREQUENCY (200)
#define VECTORNAV_PORT "/dev/ttyUSB0"
#define VECTORNAV_BAUDRATE 921600

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

template<typename... Args>
struct pack { };

// TimeStartup, TimeGPS, TimeSyncIn, Ypr, Qtn, AngularRate, Position,
// Velocity, Accel, Imu, MagPres, DeltaTheta, InsStatus, SyncInCnt, TimeGpsPps,
// TimeStartup, TimeGPS, GpsTow, GpsWeek, TimeSyncIn, TimeGpsPps, TimeUTC,
// SyncInCnt, ImuStatus, UncompMag, UncompAccel, UncompGyro, Temp, Pres,
// DeltaTheta, DeltaVel, Mag, Accel, Gyro, SensSat, Raw, UTC, Tow, Week, NumSats,
// Fix, PosLla, PosExec, VelNed, VelEcef, PosU, VelU, TimeU, SvStat,
// VpeStatus, Ypr, Qtn, DCM, MagNed, AccelNed, LinearAccelBody, LinearAccelNed,
// YprU, YprRate, StateAhrs, CovAhrs, InsStatus, PosLla, PosEcef, VelBody,
// VelNed, VelEcef, MagEcef, AccelEcef, LinearAccelEcef, PosU, VelU, StateIns, CovIns


#define IDX_TIMEGPS 0
#define IDX_YAW 1
#define IDX_ROLL 2
#define IDX_PITCH 3
#define IDX_ANGULAR_VELOCITY_X 4
#define IDX_ANGULAR_VELOCITY_Y 5
#define IDX_ANGULAR_VELOCITY_Z 6
#define IDX_LATITUDE 7
#define IDX_LONGITUDE 8
#define IDX_ALTITUDE 9
#define IDX_VELOCITY_X 10
#define IDX_VELOCITY_Y 11
#define IDX_VELOCITY_Z 12
#define IDX_ACCELERATION_X 13
#define IDX_ACCELERATION_Y 14
#define IDX_ACCELERATION_Z 15
#define IDX_TEMPERATURE 16
#define IDX_PRESSURE 17
#define IDX_MAGNETIC_X 18
#define IDX_MAGNETIC_Y 19
#define IDX_MAGNETIC_Z 20


typedef shared_memory<
  uint64_t, // gps time <0>
  float, // yaw <1>
  float, // pitch <2>
  float, // roll <3>
  float, // angular velocity x <4>
  float, // angular velocity y <5>
  float,  // angular velocity z <6>
  double, // latitude <7>
  double, // longitude <8>
  double, // altitude <9>
  float, // velocity x <10>
  float, // velocity y <11>
  float, // velocity z <12>
  float, // acceleration x <13>
  float, // acceleration y <14>
  float, // acceleration z <15>
  float, // temperature <16>
  float, // pressure <17>
  float, // magnetic x <18>
  float, // magnetic y <19>
  float // magnetic z <20>
> VNSM;

class VectorNav {
public:
  VectorNav();
  ~VectorNav();
  VNSM *get_shared_memory();
  static void packet_parser(void *, Packet &, size_t);
  float getPitch();
  float getRoll();
  static float pitch;
  static float roll;

private:
  VNSM *shared_memory;
 // DynamicFunctions d;
  static const CommonGroup commongroup = COMMONGROUP_TIMEGPS
                                          | COMMONGROUP_YAWPITCHROLL
                                          | COMMONGROUP_ANGULARRATE
                                          | COMMONGROUP_POSITION
                                          | COMMONGROUP_VELOCITY
                                          | COMMONGROUP_ACCEL;
  static const TimeGroup timegroup = TIMEGROUP_NONE;
  static const ImuGroup imugroup = IMUGROUP_TEMP
                                    | IMUGROUP_PRES
                                    | IMUGROUP_MAG;
  static const GpsGroup gpsgroup = GPSGROUP_NONE;
  static const AttitudeGroup attitudegroup = ATTITUDEGROUP_NONE;
  static const InsGroup insgroup = INSGROUP_NONE;

  const std::string vs_port = VECTORNAV_PORT;
  const uint32_t vs_baudrate = VECTORNAV_BAUDRATE;
  VnSensor vs;
};

#endif
