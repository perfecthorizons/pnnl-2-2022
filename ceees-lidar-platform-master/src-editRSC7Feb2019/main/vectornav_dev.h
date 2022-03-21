#ifndef _VECTORNAV_PACKET_T
#define _VECTORNAV_PACKET_T

#include "shared_memory.h"
#include "vn/sensors.h"
#include "dynamic_functions.h"

#include "vectornav_data_settings.h"
#include "vectornav_data.h"

#define VECTORNAV_OUTPUT_FREQUENCY (100)
#define VECTORNAV_PORT "/dev/ttyUSB0"
#define VECTORNAV_BAUDRATE 115200

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

// template<typename... Args>
// struct pack { };

typedef shared_memory<VN_DATA_TYPE> VNSM;

class VectorNav {
public:
  VectorNav();
  ~VectorNav();
  VNSM *get_shared_memory();
  static void packet_parser(void *, Packet &, size_t);
private:
  VNSM *shared_memory;
  //DynamicFunctions d;
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
