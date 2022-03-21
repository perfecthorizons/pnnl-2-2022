//vectornav.h
//last edit 7feb2019 RSC
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

#define IDX_TIMEGPS 0
#define IDX_YAW 1
#define IDX_PITCH 2
#define IDX_ROLL 3
#define IDX_QUAT0 4
#define IDX_QUAT1 5
#define IDX_QUAT2 6
#define IDX_QUAT3 7
#define IDX_LATITUDE 8
#define IDX_LONGITUDE 9
#define IDX_ALTITUDE 10
#define IDX_MAGNED0 11
#define IDX_MAGNED1 12
#define IDX_MAGNED2 13
#define IDX_LINACC0 14
#define IDX_LINACC1 15
#define IDX_LINACC2 16
#define IDX_VELNED0 17
#define IDX_VELNED1 18
#define IDX_VELNED2 19


typedef shared_memory<
  uint64_t, // IDX_TIMEGPS 0
  float, // IDX_YAW 1
  float, // IDX_PITCH 2
  float, // IDX_ROLL 3
  float, // IDX_QUAT0 4
  float, // IDX_QUAT1 5
  float,  // IDX_QUAT2 6
  float, // IDX_QUAT3 7
  double, // IDX_LATITUDE 8
  double, // IDX_LONGITUDE 9
  double, // IDX_ALTITUDE 10
  float, // IDX_MAGNED0 11
  float, // IDX_MAGNED1 12
  float, // IDX_MAGNED2 13
  float, // IDX_LINACC0 14
  float, // IDX_LINACC1 15
  float, // IDX_LINACC2 16
  float, // IDX_VELNED0 17
  float, // IDX_VELNED1 18
  float, // IDX_VELNED2 19
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
  static const CommonGroup commongroup = COMMONGROUP_TIMEGPS
                         | COMMONGROUP_YAWPITCHROLL
                         | COMMONGROUP_QUATERNION;
  static const TimeGroup timegroup = TIMEGROUP_NONE;
  static const ImuGroup imugroup = IMUGROUP_NONE;
  static const GpsGroup gpsgroup = GPSGROUP_POSLLA;
  static const AttitudeGroup attitudegroup = ATTITUDEGROUP_MAGNED
                         | ATTITUDEGROUP_ACCELNED;
  static const InsGroup insgroup = INSGROUP_VELNED;

  const std::string vs_port = VECTORNAV_PORT;
  const uint32_t vs_baudrate = VECTORNAV_BAUDRATE;
  VnSensor vs;
};

#endif
