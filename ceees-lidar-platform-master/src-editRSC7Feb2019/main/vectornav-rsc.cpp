//vectornav.cpp
//last edit 7feb2019 RSC
#include "vectornav.h"
#include <iostream>

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

float VectorNav::pitch;
float VectorNav::roll;

VectorNav::VectorNav() : vs() {
  shared_memory = new VNSM();
  shared_memory->name(
    "GpsTime",
    "Yaw", "Pitch", "Roll",
    "Quat0", "Quat1", "Quat2", "Quat3",
    "Latitude", "Longitude", "Altitude",
    "MagNED0", "MagNED1", "MagNED2",
    "LinAcc0", "LinAcc1", "LinAcc2",
    "VelNED0", "VelNED1", "VelNED2");
  vs.connect(vs_port, vs_baudrate);

  BinaryOutputRegister bor(
    ASYNCMODE_PORT1, VECTORNAV_OUTPUT_FREQUENCY,
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
        vec4f quat = p.extractVec4f();
        vec3d poslla = p.extractVec3d();
        vec3f magnetic = p.extractVec3f();
        vec3f acceleration = p.extractVec3f();
        vec3f velocity =  p.extractVec3f();
       
        pitch = ypr[2];
        roll = ypr[1];

        shared_memory->write(
          timeGPS,
          ypr[0], ypr[1], ypr[2],
          quat[0], quat[1], quat[2], quat[3],
          poslla[0], poslla[1], poslla[2],
          magnetic[0], magnetic[1], magnetic[2],
          acceleration[0], acceleration[1], acceleration[2],
          velocity[0], velocity[1], velocity[2]
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

