#include <thread>
#include <cmath>

#include "CML.h"
#include "can/can_copley.h"

#include "motorcontroller.h"
#include "debug.h"
#include "alerts.h"
#include "xsens.h"
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
    roll_profile() {

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
  MotorController::catch_error(pAmp->SetCountsPerUnit(mtr.ctsPerRev));

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
    controller_continue(true) {

  use_alerts = (alerts != nullptr);
  manualMode = false;

  hw.SetBaud(BAUD);

  MotorController::catch_error(net.Open(hw));

  cout << "opened network\n";

  settings.guardTime = 0;

  MotorController::catch_error(pitch_amplifier.Init(net, PITCH_ID, settings));
  MotorController::catch_error(roll_amplifier.Init(net, ROLL_ID, settings));

  cout << "initialized amplifier\n";

  MtrInfo mtr;
  MotorController::catch_error(pitch_amplifier.GetMtrInfo(mtr));
  MotorController::catch_error(pitch_amplifier.SetCountsPerUnit(mtr.ctsPerRev));
  MotorController::catch_error(roll_amplifier.GetMtrInfo(mtr));
  MotorController::catch_error(roll_amplifier.SetCountsPerUnit(mtr.ctsPerRev));

  MotorController::catch_error(pitch_amplifier.ClearFaults());
  MotorController::catch_error(roll_amplifier.ClearFaults());

  MotorController::catch_error(pitch_amplifier.Enable());
  MotorController::catch_error(roll_amplifier.Enable());

  pitch_profile.acc = 500;
  pitch_profile.dec = 500;
  roll_profile.acc = 500;
  roll_profile.dec = 500;

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

void MotorController::move(bool negDir) {
  if (!manualMode) {
    cout << "Must be in manual mode!\n";
    return;
  }

  //double vel = std::min(PITCH_MOTOR_SENSITIVITY * std::abs(pitch), PITCH_MOTOR_MAX_VELOCITY);
  uunit move_len = PITCH_MOTOR_MAX_VELOCITY / 500;
  if (negDir)
    move_len *= -1;

  //if (manualAmpNum == 2)
  move_len *= -1;

  pAmp->DoMove(move_len, true);
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

bool MotorController::getFaultStatus() {
  AMP_FAULT fault;
//  Error err;

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
  //status_monitor.join();
}

void MotorController::catch_error(const Error *e) {
  if (!e) return;
  LOG(1, e->toString());
  throw e->toString();
}

void MotorController::__pitch_controller() {
  std::chrono::system_clock::time_point alarm_time;
  while (controller_continue) {
    alarm_time = std::chrono::system_clock::now() + PITCH_CONTROLLER_RATE;

    //AMP_FAULT faults;
    //pitch_amplifier.GetFaults(faults);
    //if (!MotorController::parse_faults(faults, PITCH_ID)) continue;

    float pitch = xssm->read<IDX_PITCH>(); //vnsm->read<IDX_PITCH>();
    pitch_profile.vel = std::min(PITCH_MOTOR_SENSITIVITY * std::abs(pitch), PITCH_MOTOR_MAX_VELOCITY);
    //pitch_profile.vel = std::abs(PITCH_MOTOR_SENSITIVITY * pitch); //May 7, 2018
    pitch_profile.dir = (pitch >= 0) ? -1 : 1;

	 std::time_t t_read = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    double vel = std::min(PITCH_MOTOR_SENSITIVITY * std::abs(pitch), PITCH_MOTOR_MAX_VELOCITY);
    uunit move_len = vel / PITCH_CONTROLLER_RT * MOVE_LEN_FACTOR; //100 hz rate. Should use PITCH_CONTROLLER_RATE...
    if (pitch >= 0)
      move_len *= -1;

	 std::time_t before_move = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//    pitch_amplifier.DoMove(pitch_profile);
    pitch_amplifier.DoMove(move_len, true);
	 std::time_t after_move = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    alerts->diagnostic("READ pitch=%f; SET pitch_velocity=%f; move_len=%f", pitch, pitch_profile.vel, move_len);

	 std::time_t t_c = std::chrono::system_clock::to_time_t(alarm_time);
	 std::time_t t_n = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	 std::cout << "next pitch thread alarm: " << std::put_time(std::localtime(&t_c), "%F %T");
	 std::cout << "\nfinished read time:      " << std::put_time(std::localtime(&t_read), "%F %T");
	 std::cout << "\nbefore move command:     " << std::put_time(std::localtime(&before_move), "%F %T");
	 std::cout << "\nafter move command:      " << std::put_time(std::localtime(&after_move), "%F %T");
	 std::cout << "\ncurrent time:            " << std::put_time(std::localtime(&t_n), "%F %T") << std::endl;


    std::this_thread::sleep_until(alarm_time);
  }
}

void MotorController::__roll_controller() {
  std::chrono::system_clock::time_point alarm_time;
  while (controller_continue) {
    alarm_time = std::chrono::system_clock::now() + ROLL_CONTROLLER_RATE;

/*
    AMP_FAULT faults;
    roll_amplifier.GetFaults(faults);
    if (!MotorController::parse_faults(faults, ROLL_ID)) continue;
*/

    auto roll = xssm->read<IDX_ROLL>();  // vnsm->read<IDX_ROLL>();
    roll_profile.vel = std::min(ROLL_MOTOR_SENSITIVITY * std::abs(roll), ROLL_MOTOR_MAX_VELOCITY);
    roll_profile.dir = (roll >= 0) ? 1 : -1;

	 std::time_t t_read = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    double vel = std::min(ROLL_MOTOR_SENSITIVITY * std::abs(roll), ROLL_MOTOR_MAX_VELOCITY);
    uunit move_len = vel / ROLL_CONTROLLER_RT * MOVE_LEN_FACTOR; //100 hz rate. Should use ROLL_CONTROLLER_RATE...
    if (roll < 0)
      move_len *= -1;

	 std::time_t before_move = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//    roll_amplifier.DoMove(roll_profile);
    roll_amplifier.DoMove(move_len, true);

	 std::time_t after_move = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    alerts->diagnostic("READ roll=%f; SET roll_velocity=%f; move_len=%f", roll, roll_profile.vel, move_len);

	 std::time_t t_c = std::chrono::system_clock::to_time_t(alarm_time);
	 std::time_t t_n = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	 std::cout << "next roll thread alarm:  " << std::put_time(std::localtime(&t_c), "%F %T");
	 std::cout << "\nfinished read time:      " << std::put_time(std::localtime(&t_read), "%F %T");
	 std::cout << "\nbefore move command:     " << std::put_time(std::localtime(&before_move), "%F %T");
	 std::cout << "\nafter move command:      " << std::put_time(std::localtime(&after_move), "%F %T");
	 std::cout << "\ncurrent time:            " << std::put_time(std::localtime(&t_n), "%F %T") << std::endl;

    std::this_thread::sleep_until(alarm_time);
  }
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
