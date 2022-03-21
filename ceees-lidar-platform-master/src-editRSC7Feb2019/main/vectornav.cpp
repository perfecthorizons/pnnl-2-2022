#include "vectornav.h"
#include <iostream>

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

// TODO: Change to constexprs
// TODO: Setup packet parser to use dynamic transform

float VectorNav::pitch;
float VectorNav::roll;

VectorNav::VectorNav() : /*d("dyn_functions/transform.so", {"transform"}),*/ vs() {
  shared_memory = new VNSM();
  shared_memory->name(
    "gps time",
    "yaw", "pitch", "roll",
    "angular velocity x", "angular velocity y", "angular velocity z"
    "latitude", "longitude", "altitude",
    "velocity x", "velocity y", "velocity z",
    "acceleration x", "acceleration y", "acceleration z",
    "temperature",
    "pressure",
    "magnetic x", "magnetic y", "magnetic z"
  );
  vs.connect(vs_port, vs_baudrate);

  BinaryOutputRegister bor(
    ASYNCMODE_PORT1, 1000.0/VECTORNAV_OUTPUT_FREQUENCY,
    commongroup, timegroup, imugroup,
    gpsgroup, attitudegroup, insgroup
  );

  vs.writeBinaryOutput1(bor);
  vs.registerAsyncPacketReceivedHandler(
    static_cast<void *>(shared_memory),
    VectorNav::packet_parser
  );
}

VectorNav::~VectorNav() {
  vs.unregisterAsyncPacketReceivedHandler();
  vs.disconnect();
}

VNSM *VectorNav::get_shared_memory() {
  return shared_memory;
}

void VectorNav::packet_parser(void *userData, Packet &p, size_t index) {
    if (p.type() == Packet::TYPE_ASCII) {
      if (p.determineAsciiAsyncType() == VNYMR) {
        vec3f ypr, mag, accel, angularRate;
        p.parseVNYMR(&ypr, &mag, &accel, &angularRate);
      }

      return;
    } else if (p.type() == Packet::TYPE_BINARY) {
        if (!p.isCompatible(
            VectorNav::commongroup, VectorNav::timegroup,
            VectorNav::imugroup, VectorNav::gpsgroup,
            VectorNav::attitudegroup, VectorNav::insgroup)) {
          return;
        }

        VNSM *shared_memory = static_cast<VNSM *>(userData);

        uint64_t timeGPS = p.extractUint64();
        vec3f ypr = p.extractVec3f();
        vec3f angular_rate = p.extractVec3f();
        vec3d position = p.extractVec3d();
        vec3f velocity = p.extractVec3f();
        vec3f acceleration = p.extractVec3f();
        float temperature = p.extractUint32();
        float pressure = p.extractUint32();
        vec3f magnetic = p.extractVec3f();

        pitch = ypr[2];
        roll = ypr[1];

        shared_memory->write(
          timeGPS,
          ypr[0], ypr[1], ypr[2],
          angular_rate[0], angular_rate[1], angular_rate[2],
          position[0], position[1], position[2],
          velocity[0], velocity[1], velocity[2],
          acceleration[0], acceleration[1], acceleration[2],
          temperature,
          pressure,
          magnetic[0], magnetic[1], magnetic[2]
        );
    }
}

float VectorNav::getPitch() {
  return pitch;
//  return 0;
}

float VectorNav::getRoll() {
  return roll;
//  return 0;
}

