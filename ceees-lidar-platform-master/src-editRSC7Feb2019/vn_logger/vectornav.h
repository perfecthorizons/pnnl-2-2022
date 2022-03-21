#ifndef _VECTORNAV_PACKET_T
#define _VECTORNAV_PACKET_T

#include "shared_memory.h"
#include "vn/sensors.h"
#include "dynamic_functions.h"


#define VECTORNAV_OUTPUT_FREQUENCY (20)
#define VECTORNAV_PORT "/dev/ttyUSB1"
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

typedef shared_memory<
uint64_t,
float, float, float,
float, float, float, float,
double, double, double,
float, float, float,
float, float, float,
float, float, float
> VNSM;

class VectorNav {
public:
  VectorNav();
  ~VectorNav();
  VNSM *get_shared_memory();
  static void packet_parser(void *, Packet &, size_t);
private:
  VNSM *shared_memory;
 // DynamicFunctions d;
  static const CommonGroup commongroup = COMMONGROUP_TIMEGPS
                                          | COMMONGROUP_YAWPITCHROLL
                                          | COMMONGROUP_QUATERNION;
  static const TimeGroup timegroup = TIMEGROUP_NONE;
  static const ImuGroup imugroup = IMUGROUP_NONE;
  static const GpsGroup gpsgroup = GPSGROUP_POSLLA;
  static const AttitudeGroup attitudegroup = ATTITUDEGROUP_MAGNED | ATTITUDEGROUP_LINEARACCELNED;
  static const InsGroup insgroup = INSGROUP_VELNED;

  const std::string vs_port = VECTORNAV_PORT;
  const uint32_t vs_baudrate = VECTORNAV_BAUDRATE;
  VnSensor vs;
};

#endif
