#include <thread>
#include <cmath>

//#include "CML.h"
//#include "can/can_copley.h"

#include "motorcontroller.h"
#include "debug.h"
//#include "alerts.h"
//#include "xsens.h"
#include <ctime>
#include <iomanip>
#include <iostream>

MotorController::MotorController(uint16_t ampID)
  : hw(PORT),
    net(),
    pitch_amplifier(),
    roll_amplifier(),
    settings(),
    pitch_profile(),
    roll_profile(),
	 pitchGains(NULL),
	 rollGains(NULL),
	 printPitch(false),
	 printRoll(false) {

  manualMode = true;
  manualAmpNum = ampID;

  hw.SetBaud(BAUD);

  MotorController::catch_error(net.Open(hw));

  settings.guardTime = 0;

  if (ampID == PITCH_ID) {
    pAmp = &pitch_amplifier;
  }
  else {
    pAmp = &roll_amplifier;
  }

  MotorController::catch_error(pAmp->Init(net, ampID, settings));

  MtrInfo mtr;
  MotorController::catch_error(pAmp->GetMtrInfo(mtr));
  MotorController::catch_error(pAmp->SetCountsPerUnit(mtr.ctsPerRev * GEAR_RATIO / 360));

  MotorController::catch_error(pAmp->ClearFaults());

  MotorController::catch_error(pAmp->Enable());
}

MotorController::MotorController(XSSM *xssm, Alerts *alerts)
  : alerts(alerts),
    xssm(xssm),
    hw(PORT),
    net(),
    pitch_amplifier(),
    roll_amplifier(),
    settings(),
    pitch_profile(),
    roll_profile(),
    controller_continue(true),
	 printPitch(false), 
	 printRoll(false) {

  pitchGains = (Gains*) calloc(sizeof(Gains), 1);
  if (pitchGains == NULL) {
  		cout << "Failed to allocate memory for pitchGains...exiting.\n";
		exit(1);

  }
  rollGains = (Gains*) calloc(sizeof(Gains), 1);
  if (rollGains == NULL) {
  		cout << "Failed to allocate memory for rollGains...exiting.\n";
		free(pitchGains);
		exit(1);

  }

  use_alerts = (alerts != nullptr);
  manualMode = false;

  hw.SetBaud(BAUD);

  MotorController::catch_error(net.Open(hw));

  cout << "opened network\n";

  settings.guardTime = 0;

  MotorController::catch_error(pitch_amplifier.Init(net, PITCH_ID, settings));
  MotorController::catch_error(roll_amplifier.Init(net, ROLL_ID, settings));

  cout << "initialized amplifier\n";

	// Get motor info from amplifier flash and set each unit of position equal to the encoder count (4x line count)
	// times the gear ratio divided by 360 degrees (to get counts / degree platform rotation)
	// Now, each unit of position commanded = 1 degree of platform rotation
  MtrInfo mtr;
  MotorController::catch_error(pitch_amplifier.GetMtrInfo(mtr));
  MotorController::catch_error(pitch_amplifier.SetCountsPerUnit(mtr.ctsPerRev * GEAR_RATIO / 360));
  MotorController::catch_error(roll_amplifier.GetMtrInfo(mtr));
  MotorController::catch_error(roll_amplifier.SetCountsPerUnit(mtr.ctsPerRev * GEAR_RATIO / 360));

  // Clear existing amplifier faults
  MotorController::catch_error(pitch_amplifier.ClearFaults());
  MotorController::catch_error(roll_amplifier.ClearFaults());

  // Enable the amplifiers (brakes turned off here)
  MotorController::catch_error(pitch_amplifier.Enable());
  MotorController::catch_error(roll_amplifier.Enable());

  // Set acceleration profiles
  pitch_profile.acc = 400;
  pitch_profile.dec = 400;
  roll_profile.acc = 400;
  roll_profile.dec = 400;

/*
  MotorController::catch_error(pitch_amplifier.SetAmpMode(AMPMODE_CAN_VELOCITY));
  MotorController::catch_error(roll_amplifier.SetAmpMode(AMPMODE_CAN_VELOCITY));
  */

  // Start each axis controller thread
  pitch_controller = std::thread(&MotorController::__pitch_controller, this); //May 7, 2018
  roll_controller = std::thread(&MotorController::__roll_controller, this);
  //status_monitor = std::thread(&MotorController::__status_monitor, this);
    }

void MotorController::stop() {
  controller_continue = false;
}

void MotorController::clearFaults() {
  if (!manualMode) {
    cout << "Must be in manual mode!\n";
    return;
  }

  MotorController::catch_error(pAmp->ClearFaults());
}

void MotorController::move(uunit move_len, bool negDir) {
  if (!manualMode) {
    cout << "Must be in manual mode!\n";
    return;
  }

  //double vel = std::min(PITCH_MOTOR_SENSITIVITY * std::abs(pitch), PITCH_MOTOR_MAX_VELOCITY);
  //uunit move_len = .5;//PITCH_MOTOR_MAX_VELOCITY / 200;
  if (negDir)
    move_len *= -1;

  //if (manualAmpNum == 2)
  move_len *= -1;

  std::cout << "move_len: " << move_len << std::endl;

  ProfileConfigTrap move_profile;

  move_profile.acc = 200;
  move_profile.dec = 200;
  move_profile.vel = 400;
  move_profile.pos = move_len;

  pAmp->DoMove(move_profile, true);
}

bool MotorController::getBrakeStatus() {
  uint16_t outputs;

  pAmp->GetOutputs(outputs);

  return outputs & (1UL << 0);
  
}

void MotorController::setBrake(bool brakeOn) {
  uint16_t outputs;

  pAmp->GetOutputs(outputs);

  if (brakeOn)
    outputs |= (1UL);
  else
    outputs &= ~(1UL);

  pAmp->SetOutputs(outputs);
}

bool MotorController::getFaultStatus(uint16_t ampID) {
  AMP_FAULT fault;
//  Error err;


  if (ampID == PITCH_ID) {
    pAmp = &pitch_amplifier;
  }
  else {
    pAmp = &roll_amplifier;
  }

  pAmp->GetFaults(fault);

  if (fault > 0)
    return true;
  else
    return false;

//  err = pAmp->GetErrorStatus(false);
//  if  
}

MotorController::~MotorController() {
  if (!manualMode) {
    pitch_controller.join(); //May 7, 2018
    roll_controller.join();
  }

  pitch_amplifier.Disable();
  roll_amplifier.Disable();
  //status_monivel.join();
  free(pitchGains);
  free(rollGains);
}

void MotorController::catch_error(const Error *e) {
  if (!e) return;
  LOG(1, e->toString());
  throw e->toString();
}

void MotorController::setHomeLocation(uint16_t ampID) {
	
	Amp *amp = ampID == 1 ? &pitch_amplifier : &roll_amplifier;

	// Set software limits around home position
	SoftPosLimit spl;
	spl.pos = ampID == 1 ? POS_PITCH_LIMIT : POS_ROLL_LIMIT;
	spl.neg = ampID == 1 ? NEG_PITCH_LIMIT : NEG_ROLL_LIMIT;
	spl.accel = SOFT_LIMIT_ACCEL;
	spl.motorPosWrap = 0; // Disabled
	
	MotorController::catch_error(amp->SetSoftLimits(spl));

	// Set up homing move
	HomeConfig hc;
	hc.method = ampID == 1 ? CHM_HARDSTOP_NEG : CHM_HARDSTOP_POS;
	hc.velFast = 20; // uunits: platform deg
	hc.velSlow = 10;  // uunits: platform deg 
	hc.accel = 100;  // uunits: platfrom deg
	hc.offset = ampID == 1 ? PITCH_HOME_OFFSET : ROLL_HOME_OFFSET;
	hc.current = 300; // units: .01A
	hc.delay = 250;

	// Perform homing move and wait up to 60sec for it to finish
	
	//std::cout << "Homing " << (ampID == 1 ? "Pitch motor " : "Roll motor ") << std::endl;
	MotorController::catch_error(amp->GoHome(hc));
	MotorController::catch_error(amp->WaitHomeDone(60000));

}

void MotorController::getPIDGains(uint16_t ampID, Gains *gains) {
	if (gains == NULL || this->pitchGains == NULL || this->rollGains == NULL)
		return;
	
	if (ampID == 1) {
		gains->K = pitchGains->K;
		gains->Kp = pitchGains->Kp;
		gains->Ki = pitchGains->Ki;
		gains->Kd = pitchGains->Kd;
	}
	else {
		gains->K = rollGains->K;
		gains->Kp = rollGains->Kp;
		gains->Ki = rollGains->Ki;
		gains->Kd = rollGains->Kd;
	}
}

void MotorController::setPIDGains(uint16_t ampID, Gains *gains) {
	if (gains == NULL || this->pitchGains == NULL || this->rollGains == NULL)
		return;

	if (ampID == 1) {
		std::cout << "Setting Pitch Gains" << std::endl;
		pitchGains->K = gains->K;
		pitchGains->Kp = gains->Kp;
		pitchGains->Ki = gains->Ki;
		pitchGains->Kd = gains->Kd;

	}
	else {
		std::cout << "Setting Roll Gains" << std::endl;
		rollGains->K = gains->K;
		rollGains->Kp = gains->Kp;
		rollGains->Ki = gains->Ki;
		rollGains->Kd = gains->Kd;

	}
	std::cout << "gains set" <<std::endl;

}

// Used in the gain tuning thread; toggles whether the thread controlling each motor should display its updates to the terminal
void MotorController::togglePrintLiveInfo(uint16_t ampID) {
	if (ampID == 1) {
		if (this->printPitch)
			this->printPitch = false;
		else if (this->printRoll) {
			this->printRoll = false;
			this->printPitch = true;
		}
		else
			this->printPitch = true;
	}
	else {
		if (this->printRoll)
			this->printRoll = false;
		else if (this->printPitch) {
			this->printPitch = false;
			this->printRoll = true;
		}
		else
			this->printRoll = true;
	}

}

/* Use CAN SDO to get Profile Acceleration Value in units of 10 counts/s^2
 *
 * return true on success, false on error
 */
bool MotorController::getProfileAcceleration(uint16_t ampID, uint32 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld32(0x6083, 0, data);

	return err == NULL;
}

		
/* Use CAN SDO to get Profile Deceleration Value in units of 10 counts/s^2
 *
 * return true on success, false on error
 */
bool MotorController::getProfileDeceleration(uint16_t ampID, uint32 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld32(0x6084, 0, data);

	return err == NULL;
}


/* Use CAN SDO to get the V Loop velocity limit in units of .1 counts/s
 *
 * return true on success, false on error
 */
bool MotorController::getVLoopMaxVelocity(uint16_t ampID, uint32 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld32(0x2103, 0, data);

	return err == NULL;
}

/* Use CAN SDO to get the Target Velocity value in units of .1 counts/s
 * This is the velocity the SetTargetVel command is updating -- the velocity the controller is trying to command
 *
 * return true on success, false on failure
 */
bool MotorController::getTargetVelocity(uint16_t ampID, int32 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld32(0x60FF, 0, data);

	return err == NULL;
}

/* Use CAN SDO to get the Amp's Mode of Operation (1 = Profile Position, 3 = Profile Velocity, 4 = Profile Torque,
 * 6 = Homing Mode, 7 = Interpolated Position Mode ...)
 *
 * return true on success, false on failure
 */
bool MotorController::getModeOfOperation(uint16_t ampID, int8 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld8(0x6061, 0, data);

	return err == NULL;
}


/* Use CAN SDO to get Target Position in units of counts (or +/-1 to specify direction when making Velocity move
 *
 * return true on success, false on failure
 */
bool MotorController::getTargetPosition(uint16_t ampID, int32 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld32(0x607A, 0, data);

	return err == NULL;
}

/* Use CAN SDO to set the Amp's Mode of Operation (1 = Profile Position, 3 = Profile Velocity, 4 = Profile Torque,
 * 6 = Homing Mode, 7 = Interpolated Position Mode ...)
 *
 * return true on success, false on failure
 */
bool MotorController::setModeOfOperation(uint16_t ampID, int8 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Dnld8(0x6060, 0, data);

	return err == NULL;
}

bool MotorController::getAmpMode(uint16_t ampID) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	AMP_MODE mode;
	const Error *err = amp->GetAmpMode(mode);
	std::cout << "Mode: "<< mode <<std::endl;

	return err == NULL;
}

bool MotorController::getPp(uint16_t ampID, int16 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld16(0x60FB, 1, data);
	if (err != NULL) std::cout << "Error: " << err->toString() << std::endl; 

	return err == NULL;
}

bool MotorController::getVp(uint16_t ampID, int16 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Upld16(0x60F9, 1, data);
	if (err != NULL) std::cout << "Error: " << err->toString() << std::endl; 

	return err == NULL;
}

bool MotorController::setPp(uint16_t ampID, int16 &data) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->Dnld16(0x60FB, 1, data);

	return err == NULL;
}

bool MotorController::setAmpMode(uint16_t ampID, AMP_MODE mode) {
	Amp *amp = (ampID == 1) ? &pitch_amplifier : &roll_amplifier;

	if (amp == NULL) return false;

	const Error *err = amp->SetAmpMode(mode);
	// Read new amp mode to check that it worked
	const Error *err2 = amp->GetAmpMode(mode);
	std::cout << "Mode set to: "<< mode <<std::endl;

	return err == NULL && err2 == NULL;
}

void MotorController::__pitch_controller() {
  
  setHomeLocation(1);

  setAmpMode(1, AMPMODE_CAN_VELOCITY);

  std::fstream fs_pitch;
  fs_pitch.open("/home/pi/pitch_data.csv", std::fstream::out);
  //std::chrono::system_clock::time_point alarm_time;
  auto time1 = std::chrono::high_resolution_clock::now();
  auto time2 = std::chrono::high_resolution_clock::now();

  double pitchIntegral = 0;
  double pitchDerivative = 0;

  pitchGains->K = 1.; //.1; //1;

  pitchGains->Kp = 8; //69; //3;
  pitchGains->Ki = .1;//1; //.7; //1 //0; 
  pitchGains->Kd = 0;//6.9; //3.6; //6.9; //36;

  //working:: 1, 6.0, 1, 6.9

  double lastPitchError = xssm->read<IDX_PITCH>();

  //double velocities[] = {0,0,0};
  pitch_amplifier.SetProfileAcc(1000);
  pitch_amplifier.SetProfileDec(1000);

  while (controller_continue) {
    //alarm_time = std::chrono::system_clock::now() + PITCH_CONTROLLER_RATE;

	 time2 = std::chrono::high_resolution_clock::now();

	 std::chrono::duration<double> elapsed = time2 - time1;
	 double deltaT = elapsed.count();

	 time1 = time2;

/*
    AMP_FAULT faults;
    roll_amplifier.GetFaults(faults);
    if (!MotorController::parse_faults(faults, PITCH_ID)) continue;
	 */

	 // Read pitch in degrees from shared memory
    float pitchError = xssm->read<IDX_PITCH>();  // vnsm->read<IDX_ROLL>();
	 float absPitchError = std::abs(pitchError);

	 //double roll_target_pos = roll
	 //roll_profile.pos = ROLL_MOVE_FACTOR * roll;

	double dampingFactor = absPitchError < .2 ? absPitchError / .2 : 1; //absPitchError < 1.98 ? absPitchError * absPitchError / 4. + .02 : 1;
		

	//roll_profile.dir = rollError > 0 ? 1 : -1;

	pitchDerivative = (pitchError - lastPitchError) / deltaT; 

	pitchIntegral += pitchError * deltaT;

	double vel = dampingFactor * pitchGains->K * 
		(pitchError * pitchGains->Kp 
		+ pitchIntegral * pitchGains->Ki 
		+ pitchDerivative * pitchGains->Kd);

	pitch_profile.dir = vel < 0 ? 10 : -10;

	// TODO: sort up this messed up bounds thing
	if (std::abs(vel) > PITCH_MOTOR_MAX_VELOCITY) {
		vel = (vel < 0) ? -1 * PITCH_MOTOR_MAX_VELOCITY : PITCH_MOTOR_MAX_VELOCITY;
		if (std::abs(pitchGains->K * pitchGains->Ki * pitchIntegral) > PITCH_MOTOR_MAX_VELOCITY) {
			pitchIntegral = pitchIntegral > 0 ? PITCH_MOTOR_MAX_VELOCITY / (pitchGains->Ki * pitchGains->K) : -1 * PITCH_MOTOR_MAX_VELOCITY / (pitchGains->Ki * pitchGains->K);
		}
	}

/*
	velocities[2] = velocities[1];
	velocities[1] = velocities[0];
	velocities[0] = vel;
	*/

	pitch_profile.vel = std::abs(vel); //velocities[0] + velocities[1] + velocities[2]) / 3;
	 
   //pitch_amplifier.DoMove(pitch_profile);
	vel *= -1;
	pitch_amplifier.SetTargetVel(vel);

    //alerts->diagnostic("READ pitch=%f; READ deltaT=%f; SET dir=%f; SET pitch_vel=%f; SET integral=%f", pitchError, deltaT, pitch_profile.dir, pitch_profile.vel, pitchIntegral);
	 
	 if (this->printPitch) {
		std::cout << setprecision(5);
		std::cout << "\r" << "> Pitch: " << setw(8) << pitchError;
		std::cout << ", Dir: " << setw(4) << pitch_profile.dir;
		std::cout << ", Pitch Velocity: " << setw(8) << vel; //pitch_profile.vel;
		std::cout << ", Pitch Integral: " << setw(8) << pitchIntegral;
		std::cout << ", DeltaT: " << setw(8) << deltaT;;
		std::cout << std::flush;

	}

	 fs_pitch << pitchError << "," << vel << "," << deltaT << std::endl;
	 lastPitchError = pitchError;


	 std::this_thread::yield();

  }

  fs_pitch.close();
}

void MotorController::__roll_controller() {


	// Set current location as 0 point
	setHomeLocation(2);

  setAmpMode(2, AMPMODE_CAN_VELOCITY);

  std::fstream fs_roll;
  fs_roll.open("/home/pi/roll_data.csv", std::fstream::out);
 // std::chrono::system_clock::time_point alarm_time;
  auto time1 = std::chrono::high_resolution_clock::now();
  auto time2 = std::chrono::high_resolution_clock::now();

  double rollIntegral = 0;
  double rollDerivative = 0;

  rollGains->K = .2;//.1738; //3.69;//.1738;

  rollGains->Kp = 69;//42; //169; // 69;
  rollGains->Ki = 1;//42.0; //69; //154.20; // 42.0; 
  rollGains->Kd = 0;//46; //4.20;//36;

  double lastRollError = xssm->read<IDX_ROLL>();
  roll_amplifier.SetProfileAcc(1000);
  roll_amplifier.SetProfileDec(1000);

  while (controller_continue) {
    //alarm_time = std::chrono::system_clock::now() + ROLL_CONTROLLER_RATE;

	 time2 = std::chrono::high_resolution_clock::now();

	 std::chrono::duration<double> elapsed = time2 - time1;
	 double deltaT = elapsed.count();

	 time1 = time2;

/*
    AMP_FAULT faults;
    roll_amplifier.GetFaults(faults);
    if (!MotorController::parse_faults(faults, ROLL_ID)) continue;
	 */

	 // Read roll in degrees from shared memory
    float rollError = xssm->read<IDX_ROLL>();  // vnsm->read<IDX_ROLL>();
	 float absRollError = std::abs(rollError);

	 //double roll_target_pos = roll
	 //roll_profile.pos = ROLL_MOVE_FACTOR * roll;

	double dampingFactor = absRollError < .2 ? absRollError / .2 : 1; //absRollError < 1.98 ? absRollError * absRollError / 4. + .02 : 1;
		

	//roll_profile.dir = rollError > 0 ? 1 : -1;

	rollDerivative = (rollError - lastRollError) / deltaT; 

	rollIntegral += rollError * deltaT;

	double vel = dampingFactor * rollGains->K * 
		(rollError * rollGains->Kp 
		+ rollIntegral * rollGains->Ki 
		+ rollDerivative * rollGains->Kd);

	roll_profile.dir = vel > 0 ? 10 : -10;

	if (std::abs(vel) > ROLL_MOTOR_MAX_VELOCITY) {
		vel = (vel < 0) ? -1 * ROLL_MOTOR_MAX_VELOCITY : ROLL_MOTOR_MAX_VELOCITY;
		//vel = ROLL_MOTOR_MAX_VELOCITY; //roll_profile.vel > 0 ? ROLL_MOTOR_MAX_VELOCITY : -1 * ROLL_MOTOR_MAX_VELOCITY;
		if (std::abs(rollGains->K * rollGains->Ki * rollIntegral) > ROLL_MOTOR_MAX_VELOCITY) {
			rollIntegral = rollIntegral > 0 ? ROLL_MOTOR_MAX_VELOCITY / (rollGains->Ki * rollGains->K) : -1 * ROLL_MOTOR_MAX_VELOCITY / (rollGains->Ki * rollGains->K);
		}
	}

	roll_profile.vel = std::abs(vel);


	/*  PROPORTIONAL GAIN 
	if (absRoll < 10) {
		roll_profile.vel = 10*absRoll;

	}
	else {
		roll_profile.vel = ROLL_MOTOR_MAX_VELOCITY;
	}
	*/
	 
   //roll_amplifier.DoMove(roll_profile);

	roll_amplifier.SetTargetVel(vel);

	//roll_amplifier.SetTargetPos(roll_target_pos);
   //alerts->diagnostic("READ roll=%f; READ deltaT=%f; SET dir=%f; SET roll_vel=%f; SET integral=%f", rollError, deltaT, roll_profile.dir, roll_profile.vel, rollIntegral);
   //alerts->diagnostic("READ roll=%f; READ deltaT=%f; SET dir=%f; SET roll_vel=%f; SET integral=%f", rollError, deltaT, roll_profile.dir, roll_profile.vel, rollIntegral);
	if (this->printRoll) {
		std::cout << setprecision(5);
		std::cout << "\r" << "> Roll: " << setw(8) << rollError;
		std::cout << ", Dir: " << setw(4) << roll_profile.dir;
		std::cout << ", Roll Velocity: " << setw(8) << roll_profile.vel;
		std::cout << ", Roll Integral: " << setw(8) << rollIntegral;
		std::cout << ", DeltaT: " << setw(8) << deltaT;
		std::cout <<std::flush;
	}
	 
	 lastRollError = rollError;

	 fs_roll << rollError << "," << vel << "," << deltaT << std::endl;

	 std::this_thread::yield();
    //std::this_thread::sleep_until(alarm_time);
  }

  fs_roll.close();
}

void MotorController::__status_monitor() {
  while (controller_continue) {
    AMP_MODE roll_mode, pitch_mode;
    roll_amplifier.GetAmpMode(roll_mode);
    pitch_amplifier.GetAmpMode(pitch_mode);

    int16_t roll_temperature, pitch_temperature;
    roll_amplifier.GetAmpTemp(roll_temperature);
    pitch_amplifier.GetAmpTemp(pitch_temperature);

    int16_t roll_amperage, pitch_amperage;
    roll_amplifier.GetCurrentActual(roll_amperage);
    pitch_amplifier.GetCurrentActual(pitch_amperage);

    int16_t roll_bus_voltage, pitch_bus_voltage;
    roll_amplifier.GetHighVoltage(roll_bus_voltage);
    pitch_amplifier.GetHighVoltage(pitch_bus_voltage);

    std::this_thread::sleep_for(STATUS_POLLING_RATE);
  }
}

bool MotorController::parse_faults(AMP_FAULT &faults, uint16_t which_amplifer) {
  unsigned fault_count = 0;

  // TODO: attempt to automatically resolve faults

  if (faults & FAULT_DATAFLASH) {
    ++fault_count;
    alerts->fatal_error("The flash data in amplifer %u is corrupt.", which_amplifer);
  }

  if (faults & FAULT_ADCOFFSET) {
    ++fault_count;
    alerts->fatal_error("An A/D offset error in amplifier %u has occurred.", which_amplifer);
  }

  if (faults & FAULT_SHORT_CRCT) {
    ++fault_count;
    alerts->fatal_error("A short circuit condition was detected on amplifier %u.", which_amplifer);
  }

  if (faults & FAULT_AMP_TEMP) {
    ++fault_count;
    alerts->nonfatal_error("Amplifier %u is over temperature.", which_amplifer);
  }

  if (faults & FAULT_MTR_TEMP) {
    ++fault_count;
    alerts->nonfatal_error("A motor temperature error was detected by amplifier %u.", which_amplifer);
  }

  if (faults & FAULT_OVER_VOLT) {
    ++fault_count;
    alerts->nonfatal_error("The bus voltage on amplifier %u is over the acceptable limit.", which_amplifer);
  }

  if (faults & FAULT_UNDER_VOLT) {
    ++fault_count;
    alerts->nonfatal_error("The bus voltage on amplifier %u is below the acceptable limit.", which_amplifer);
  }

  if (faults & FAULT_ENCODER_PWR) {
    ++fault_count;
    alerts->nonfatal_error("Amplifier %u detected over current conditions on the encoder power supply.", which_amplifer);
  }

  if (faults & FAULT_PHASE_ERR) {
    ++fault_count;
    alerts->fatal_error("Amplifier %u detected a phasing error.", which_amplifer);
  }

  if (faults & FAULT_TRK_ERR) {
    ++fault_count;
    alerts->nonfatal_error("Amplifier %u detected a tracking error.", which_amplifer);
  }

  if (faults & FAULT_I2T_ERR) {
    ++fault_count;
    alerts->warning("Current limited by i^2t algorithm on amplifier %u.", which_amplifer);
  }

  return (fault_count == 0);
}
