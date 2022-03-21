#ifndef _MOTORCONTROLLER_H
#define _MOTORCONTROLLER_H

#include <chrono>

#include "CML.h"
#include "can/can_copley.h"

//#include "shared_memory.h"
//#include "dynamic_functions.h"

#include "movingavg.h"
#include "literals.h"
#include "alerts.h"
//#include "xsens.h"
#include <fstream>
#include <thread>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

struct Gains {
	double K;
	double Kp;
	double Ki;
	double Kd;
};

struct AxisData {
	double K;
	double Kp;
	double Ki;
	double Kd;
	float  lastError;
	double integral;
	double derivative;
};

constexpr static const uint32_t BAUD = 1000000;
constexpr static const char *PORT = "CAN0";

constexpr uint16_t PITCH_ID = 1;
constexpr uint16_t ROLL_ID = 2;

constexpr static const double PITCH_MOTOR_SENSITIVITY = 1.0;
constexpr static const double ROLL_MOTOR_SENSITIVITY = 1.0;
constexpr static const double PITCH_MOTOR_MAX_VELOCITY = 50.0;
constexpr static const double ROLL_MOTOR_MAX_VELOCITY = 50.0;

// old
constexpr static const double PITCH_MOVE_FACTOR = -1;
constexpr static const double ROLL_MOVE_FACTOR = 1;

constexpr static const std::chrono::nanoseconds PITCH_CONTROLLER_RATE = 400_hz; //100_hz;
constexpr static const std::chrono::nanoseconds ROLL_CONTROLLER_RATE = 400_hz; //100_hz;
constexpr static const std::chrono::nanoseconds STATUS_POLLING_RATE = 50_hz;

// Degree Movement limits (relative to 0 being the center)
constexpr static const double NEG_ROLL_LIMIT = -27; //-125000;
constexpr static const double POS_ROLL_LIMIT = 27; //125000;
constexpr static const double NEG_PITCH_LIMIT = -25; //-120000;
constexpr static const double POS_PITCH_LIMIT = 25; //120000;

constexpr static const double SOFT_LIMIT_ACCEL = 0; // 0 = cuts off current at motion limit (like a limit switch); 

// Motor Parameters
//constexpr static const int ENCODER_COUNT = 4096;
constexpr static const double GEAR_RATIO = 100;

// Homing parameters
constexpr static const double PITCH_HOME_OFFSET = 43.94; //200000;
constexpr static const double ROLL_HOME_OFFSET = -61.52; //-280000;
//COPLEY_HOME_METHOD PITCH_HOME_METHOD = CHM_HARDSTOP_NEG;
//COPLEY_HOME_METHOD ROLL_HOME_METHOD = CHM_HARDSTOP_POS;

constexpr static const double X_DAMPING_LIMIT = 1;
constexpr static const double Y_DAMPING_LIMIT = 1;


CML_NAMESPACE_USE();

class MotorController {
public:
  //MotorController(XSSM *xssm, Alerts *);
  MotorController(Alerts *);
  MotorController(uint16_t ampID);
  ~MotorController();
  void stop();
  void move(uunit movelen, bool negDir);
  bool getBrakeStatus();
  void setBrake(bool brakeOn);
  bool getFaultStatus(uint16_t ampID);
  void clearFaults();
  void clearFaults(uint16_t ampID);
  void setHomeLocation(uint16_t ampID);
  void getPIDGains(uint16_t ampID, Gains *gains);
  void setPIDGains(uint16_t ampID, Gains *gains);
  void togglePrintLiveInfo(uint16_t ampID);

  // CAN object access functions
  bool getProfileAcceleration(uint16_t ampID, uint32 &data);
  bool getProfileDeceleration(uint16_t ampID, uint32 &data);
  bool getVLoopMaxVelocity(uint16_t ampID, uint32 &data);
  bool getTargetVelocity(uint16_t ampID, int32 &data);
  bool getModeOfOperation(uint16_t ampID, int8 &data);
  bool getTargetPosition(uint16_t ampID, int32 &data);

  bool getPp(uint16_t ampID, int16 &data);
  bool getVp(uint16_t ampID, int16 &data);
  bool setPp(uint16_t ampID, int16 &data);

  bool getAmpMode(uint16_t ampID);
  bool setAmpMode(uint16_t ampID, AMP_MODE mode);

  bool saveConfigToFlash(uint16_t ampID);


	// Functions staying in -- overhaul
	void setupPitchControl();
	void setupRollControl();

	void correctInclination(float pitchError, float rollError); 
	void correctPitch(float pitchError);
	void correctRoll(float rollError);

private:
  Alerts *alerts;
  bool use_alerts;
  bool manualMode;
  uint16_t manualAmpNum;

  //XSSM *xssm;

  static void catch_error(const Error *);

  CopleyCAN hw;
  CanOpen net;
  Amp pitch_amplifier, roll_amplifier;
  Amp* pAmp;
  AmpSettings settings;
  ProfileConfigVel pitch_profile, roll_profile;
  bool controller_continue;
  Gains *pitchGains, *rollGains;
  bool printPitch, printRoll;

  void __pitch_controller();
  void __roll_controller();
  void __status_monitor();

  bool parse_faults(AMP_FAULT &, uint16_t);

  //std::thread pitch_controller;
  //std::thread roll_controller;
  //std::thread status_monitor;
  bool setModeOfOperation(uint16_t ampID, int8 &data);

	bool pitchActive, rollActive;
  MovingAvg<double> pitchMovingAvg;
  MovingAvg<double> rollMovingAvg;

	static AxisData pitchData, rollData;

	static std::chrono::time_point<std::chrono::high_resolution_clock> ptime1, ptime2;
	static std::chrono::time_point<std::chrono::high_resolution_clock> rtime1, rtime2;

};

#endif
