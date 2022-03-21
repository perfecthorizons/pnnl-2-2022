#ifndef _MOTORCONTROLLER_H
#define _MOTORCONTROLLER_H

#include <chrono>

#include "CML.h"
#include "can/can_copley.h"

#include "shared_memory.h"
#include "vectornav.h"
#include "dynamic_functions.h"
#include "literals.h"
#include "alerts.h"

constexpr static const uint32_t BAUD = 1000000;
constexpr static const char *PORT = "CAN0";

constexpr uint16_t PITCH_ID = 1;
constexpr uint16_t ROLL_ID = 2;

constexpr static const double PITCH_MOTOR_SENSITIVITY = 1.2 / 2.0;
constexpr static const double ROLL_MOTOR_SENSITIVITY = 1.2 / 2.0;
constexpr static const double PITCH_MOTOR_MAX_VELOCITY = 20.0;
constexpr static const double ROLL_MOTOR_MAX_VELOCITY = 20.0;

constexpr static const std::chrono::nanoseconds PITCH_CONTROLLER_RATE = 1000_hz; //100_hz;
constexpr static const std::chrono::nanoseconds ROLL_CONTROLLER_RATE = 1000_hz; //100_hz;
constexpr static const std::chrono::nanoseconds STATUS_POLLING_RATE = 50_hz;

CML_NAMESPACE_USE();

class MotorController {
public:
  MotorController(VNSM *vnsm, Alerts *);
  MotorController(uint16_t ampID);
  ~MotorController();
  void stop();
  void move(bool negDir);
  bool getBrakeStatus();
  void setBrake(bool brakeOn);
  bool getFaultStatus();
  void clearFaults();

private:
  Alerts *alerts;
  bool use_alerts;
  bool manualMode;
  uint16_t manualAmpNum;

  VNSM *vnsm;

  static void catch_error(const Error *);

  CopleyCAN hw;
  CanOpen net;
  Amp pitch_amplifier, roll_amplifier;
  Amp* pAmp;
  AmpSettings settings;
  ProfileConfigVel pitch_profile, roll_profile;
  bool controller_continue;

  void __pitch_controller();
  void __roll_controller();
  void __status_monitor();

  bool parse_faults(AMP_FAULT &, uint16_t);

  std::thread pitch_controller;
  std::thread roll_controller;
  std::thread status_monitor;
};

#endif
