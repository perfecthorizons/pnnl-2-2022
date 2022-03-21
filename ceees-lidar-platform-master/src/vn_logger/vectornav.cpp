#include "vectornav.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;

// TODO: Change to constexprs
// TODO: Setup packet parser to use dynamic transform

VectorNav::VectorNav() : /*d("dyn_functions/transform.so", {"transform"}),*/ vs() {
  shared_memory = new VNSM();
  shared_memory->name(
    "timeGPS",
    "ypr0", "ypr1", "ypr2",
    "quat0", "quat1", "quat2", "quat3",
    "pos0", "pos1", "pos2",
    "mag0", "mag1", "mag2",
    "lin_acc0", "lin_acc1", "lin_acc2",
    "vel0", "vel1", "vel2"
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
        vec3d pos = p.extractVec3d();
        vec3f mag = p.extractVec3f();
        vec3f lin_acc = p.extractVec3f();
        vec3f vel = p.extractVec3f();

        shared_memory->write(
            timeGPS,
            ypr[0], ypr[1], ypr[2],
            quat[0], quat[1], quat[2], quat[3],
            pos[0], pos[1], pos[2],
            mag[0], mag[1], mag[2],
            lin_acc[0], lin_acc[1], lin_acc[2],
            vel[0], vel[1], vel[2]
        );
    }
}
