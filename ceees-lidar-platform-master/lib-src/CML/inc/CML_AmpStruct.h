/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file

  This file contains a number of structures used to pass configuration 
  parameters to an Amp object.

*/

#ifndef _DEF_INC_CML_AMPSTRUCT
#define _DEF_INC_CML_AMPSTRUCT

#include "CML_Error.h"
#include "CML_Filter.h"
#include "CML_InputShaper.h"
#include "CML_Settings.h"
#include "CML_Utils.h"

CML_NAMESPACE_START()

/***************************************************************************/
/**
This class represents error conditions that can occur when loading amplifer
data from a data file.
*/
/***************************************************************************/
class AmpFileError: public Error
{
public:
   static const AmpFileError format;       ///< Amplifier file format error
   static const AmpFileError tooOld;       ///< Amplifier file format is too old, use CME version 3.1 or later
   static const AmpFileError noFileAccess; ///< File access was not enabled at compile time.  See CML_Settings.h
   static const AmpFileError fileOpen;     ///< Error opening amplifier file
   static const AmpFileError range;        ///< A parameter in the amplifier file is out of range
   static const AmpFileError axis;         ///< Amplifier file is for multi axis, not supported
   static const AmpFileError axisCt;       ///< Amplifier requested an axis that is not present in the ccx file or vice versa
   static const AmpFileError FPGAtooOld;   ///< Amplifier firmware is too old, update to 2.72 or later.

protected:
   /// Standard protected constructor
   AmpFileError( uint16 id, const char *desc ): Error( id, desc ){}
};

/***************************************************************************/
/**
  Amplifier characteristics data structure.

  This structure is used to hold information about the amplifier such as it's
  model number, serial number, peak current rating, etc.  

  The amplifier characteristics defined in this structure can not be changed.
  They are defined by Copley Controls when the amplifier is designed and/or
  manufactured.

  Use the Amp::GetAmpInfo method to retrieve this information from the amplifier.

*/
/***************************************************************************/
struct AmpInfo
{
#define COPLEY_MAX_STRING    41

   char model[ COPLEY_MAX_STRING ];        ///< Model number string
   char mfgName[ COPLEY_MAX_STRING ];      ///< Name of the amplifier manufacturer
   char mfgWeb[ COPLEY_MAX_STRING ];       ///< Web address of the manufacturer
   char mfgInfo[ COPLEY_MAX_STRING ];      ///< Amplifier's manufacturing information string
   char swVer[ COPLEY_MAX_STRING ];        ///< Software version number

   uint16 swVerNum;      ///< Version number represented as an integer

   uint32 serial;        ///< Serial number
   uint32 modes;         ///< Supported modes of operation (see DSP402 spec)

   uint16 crntPeak;      ///< Peak current rating (10 milliamp units)
   uint16 crntCont;      ///< Continuous current rating (10 milliamp units)
   uint16 crntTime;      ///< Time at peak current (milliseconds)

   uint16 voltMax;       ///< Max bus voltage (100 millivolt units)
   uint16 voltMin;       ///< Min bus voltage (100 millivolt units)
   uint16 voltHyst;      ///< Bus voltage hysteresis for over voltage shutdown (100 millivolt units)

   uint16 tempMax;       ///< Max temperature (deg C)
   uint16 tempHyst;      ///< Temperature hysteresis for over temp shutdown (deg C)

   uint16 pwmPeriod;     ///< PWM period (10 nanosecond units)
   uint16 servoPeriod;   ///< Servo period (multiples of PWM period)

   // The following values or primarily for internal use by Copley
   int16 crntScale;      ///< Current scaling factor 
   int16 voltScale;      ///< Voltage scaling factor
   int16 refScale;       ///< Reference scaling factor
   int16 aencScale;      ///< Analog encoder scaling factor
   int16 type;           ///< Amp type
   int16 pwm_off;        ///< PWM off time
   int16 pwm_dbzero;     ///< PWM deadband @ zero current
   int16 pwm_dbcont;     ///< PWM deadband @ continuous current
   int16 regenPeak;      ///< Internal regen resister peak limit
   int16 regenCont;      ///< Internal regen resister continuous limit
   int16 regenTime;      ///< Internal regen resister peak time
};

/***************************************************************************/
/**
  This structure holds the position loop configuration parameters specific to 
  the Copley amplifier.

  The position loop is one of three servo control loops used by the amplifier
  to control a motor.  The configuration parameters used by this control loop
  allow the servo performance to be 'tuned' for various motors and loads.

  The amplifier member functions Amp::GetPosLoopConfig and Amp::SetPosLoopConfig
  are used to read and write this data to the amplifier.
  */
/***************************************************************************/
struct PosLoopConfig
{
   /// Proportional gain
   int16 kp;

   /// Velocity feed forward
   int16 kvff;

   /// Acceleration feed forward
   int16 kaff;

   /// Integral gain
   int16 ki;

   /// Derivative gain
   int16 kd;

   /// Scaling factor.  This is a multiplier that is applied to the
   /// output of the position loop.  It's scaled up by 100, so setting
   /// the scaling factor to 1234 would multiply the output of the 
   /// loop by 12.34.
   /// This parameter was added in firmware version 3.30.  For any 
   /// earlier version it will default to 100 (scale by 1.0).
   int16 scale;

   /// cross coupling derivative gain
   int16 xKd;

   /// cross coupling integral gain
   int16 xKi;

   /// cross coupling proportional gain
   int16 xKp;

   /// Position loop drain value for integral sum. Set to 0 to disable
   int16 kiDrain;

   /// Default constructor.
   /// Simply initializes all servo parameters to zero.
   PosLoopConfig( void )
   {
      kp = 0;
      ki = 0;
      kd = 0;
      kvff = 0;
      kaff = 0;
      scale = 100;
      xKd = 0;
      xKi = 0;
      xKp = 0;
      kiDrain = 0;
   }
};

/***************************************************************************/
/**
  This structure holds configuration info about specific parts of the velocity
  and position loops.
  */
/***************************************************************************/
struct ServoLoopConfig
{
   /// Servo Loop Configuration
   /// This paramater it set up as follows:
   /// Bit 0:    If set, disables the velocity loop gains. The velocity
   ///           loop command feed forward gain is still active as are
   ///           the velocity loop output filters.
   /// Bit 1:    If set, this enables the position loop I and D gains.
   ///           If clear, these params are treated as zeros.
   /// Bit 2-31: Reserved for future use
   int32 servoLoopConfig;

   ServoLoopConfig( void )
   {
      servoLoopConfig = 0;
   }
};

/***************************************************************************/
/**
This structure holds configuration info about the encoder error filter.
*/
/***************************************************************************/
struct EncoderErrorConfig
{
	/// Encoder Error Filter Configuration
	/// This parameter it set up as follows:
	/// Bits 0-3:   Maximum number of consecutice bad samples to allow.
	/// Bits 4-15:  Reserved for future use.
	/// Bits 16-28: Maximum difference between the actual and 
	///			    extrapolated reading before treating reading as bad.
	/// Bits 29-31: Reserved for future use.
	uint32 encoderErrorConfig;

	EncoderErrorConfig(void)
	{
		encoderErrorConfig = 0x00640003;
	}
};

/***************************************************************************/
/**
  This structure holds the velocity loop configuration parameters 
  specific to the Copley amplifier.

  The velocity loop is one of three servo control loops used by the amplifier
  to control a motor.  The configuration parameters used by this control loop
  allow the servo performance to be 'tuned' for various motors and loads.

  The amplifier member functions Amp::GetVelLoopConfig and Amp::SetVelLoopConfig
  are used to read and write this data to the amplifier.
  */
/***************************************************************************/
struct VelLoopConfig
{
   /// Proportional gain
   int16 kp;

   /// Integral gain
   int16 ki;

   /// Acceleration feed forward
   int16 kaff;

   /// Velocity loop command feed forward
   /// The input command (after limiting) to the velocity loop is
   /// scaled by this value and added to the output of the velocity
   /// loop.
   int16 velCmdff;

   /// Output shift value.  The output of the loop is downshifted
   /// this many bits to get the current loop command
   int16 shift;

   /// Velocity loop drain value for integral sum. Set to 0 to disable
   int16 viDrain;

   /// Maximum allowed velocity.
   /// This value is used to limit the velocity command before the
   /// velocity loop uses it to calculate output current.
   /// When running in a position mode (normal for CAN operation)
   /// The velocity command is the output from the position loop.
   /// This command is clipped by this value before it is passed
   /// to the velocity loop.
   ///
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit maxVel;

   /// Maximum allowed acceleration
   ///
   /// This value limits the rate of change of the velocity command
   /// input to the velocity loop.  It is used when the magnitude of
   /// the command is increasing.
   ///
   /// Note that the acceleration & deceleration limits are NOT used
   /// when the position loop is driving the velocity loop.
   ///
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit maxAcc;

   /// Maximum allowed deceleration
   /// This value limits the rate of change of the velocity command
   /// input to the velocity loop.  It is used when the magnitude of
   /// the command is decreasing.
   ///
   /// Note that the acceleration & deceleration limits are not used
   /// when the position loop is driving the velocity loop.  
   ///
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit maxDec;

   /// Deceleration used for emergency stop
   /// When the position loop is driving the velocity loop this value 
   /// is only used for tracking error conditions.  If a tracking error
   /// occurs, the velocity loop takes over control and drives to zero
   /// velocity using this deceleration value.
   ///
   /// Setting this value to zero indicates that the deceleration is
   /// unlimited.
   ///
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit estopDec;

   /// Default constructor.
   /// Simply initializes all members to zero.
   VelLoopConfig( void )
   {
      kp = 0;
      ki = 0;
      kaff = 0;
      velCmdff = 0;
      shift = 0;
      viDrain = 0;
      maxVel = 0;
      maxAcc = 0;
      maxDec = 0;
      estopDec = 0;
   }
};

/***************************************************************************/
/**
  This structure holds the current loop configuration parameters. 

  The current loop is one of three servo control loops used by the amplifier
  to control a motor.  The configuration parameters used by this control loop
  allow the servo performance to be 'tuned' for various motors and loads.

  This structure also holds the parameters used to control current limiting.
  The current limiting acts on the commanded current before it is sent to the
  current loop.

  The amplifier member functions Amp::GetCrntLoopConfig and Amp::SetCrntLoopConfig
  are used to read and write this data to the amplifier.
  */
/***************************************************************************/
struct CrntLoopConfig
{
   /// Proportional gain
   int16 kp;

   /// Integral gain
   int16 ki;

   /// Current offset
   int16 offset;

   /// Peak current limit (0.01 amp units)
   /// This is the maximum current that can be applied to the 
   /// motor at any time
   /// Also used as Boost current in stepper mode
   int16 peakLim;

   /// Continuous current limit (0.01 amp units)
   /// This is the maximum current that can continuously be 
   /// applied to the load.
   /// Also used as Run current in stepper mode.
   int16 contLim;

   /// Time at peak current limit (milliseconds)
   /// If peak current is requested, it will fall back to the
   /// continuous limit within this amount of time.
   /// Also used as Boost current time in stepper mode.
   int16 peakTime;

   /// Stepper hold current (0.01 amps).
   /// Current used to hold the motor at rest.
   /// Used in stepper mode only.
   uint16 stepHoldCurrent;

   /// Run to hold time(milliseconds)
   /// The period of time, beginning when a move is complete,
   /// to when output current switched to hold current.
   /// Used in stepper mode only.
   uint16 stepRun2HoldTime;

   /// Voltage control mode time delay (milliseconds)
   /// Time delay to enter into a special voltage control mode.
   /// If set to zero this feature is disabled.
   /// Used for stepper mode only.
   uint16 stepVolControlDelayTime;

   /// Rate of change of current command (milliamps/sec).
   /// This parameter is only used when running in the low level
   /// programmed current mode (AMPMODE_PROG_CRNT), or in the 
   /// CANopen profile torque mode (AMPMODE_CAN_TORQUE).  In 
   /// other modes this parameter is ignored and no limit is placed
   /// on the slope of the current command.
   ///
   /// If this parameter is set to zero (default) it is not used
   /// in any mode of operation.
   ///
   /// Note that this parameter is internally the same as the torque
   /// slope parameter which can be set using the function 
   /// Amp::SetTorqueSlope.  The units are different however as this 
   /// parameter controls slope in units of current and the torque slope
   /// function adjusts in units of torque.
   int32 slope;

   /// Default constructor.
   /// Simply initializes all members to zero.
   CrntLoopConfig( void )
   {
      kp = 0;
      ki = 0;
      offset = 0;
      slope = 0;

      peakLim = 0;
      contLim = 0;
      peakTime = 0;

      stepHoldCurrent = 0;
      stepRun2HoldTime = 0;
      stepVolControlDelayTime = 0;
   }
};


/***************************************************************************/
/**
  Homing parameter structure.  This structure allows all homing parameters 
  to be grouped together and passed to the amplifier for convenience.
  */
/***************************************************************************/
struct HomeConfig
{
   /// Homing method to use
   COPLEY_HOME_METHOD method;

   /// Extended home method.  If the main home method is set to 
   /// any value other then 'CHM_EXTENDED' then this parameter
   /// is ignored.  If the home method is set to this value, then
   /// this value will be used to define the low level homing 
   /// routine used by the amplifier.
   ///
   /// For the most part this parameter can be ignored.  It's 
   /// intended to allow access to some low level features of 
   /// the amplifier's homing state machine which are otherwise
   /// not available through the more generic home methods.
   /// 
   /// Encodings for this parameter can be found in the CANopen
   /// programmers guide for CANopen object 0x2352, or in the 
   /// serial port programmers guide for variable 0xC2.
   uint16 extended;

   /// Velocity to use for fast moves during the home procedure.
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit velFast;

   /// Velocity to use when seeking a sensor edge.
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit velSlow;

   /// Acceleration to use for the home procuedure.
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit accel;

   /// Offset from located home position to zero position.
   /// After the home position is found as defined by the home method,
   /// this offset will be added to it and the resulting position will
   /// be considered the zero position.
   /// This parameter is specified in "user units".  See 
   /// Amp::SetCountsPerUnit for details.
   uunit offset;

   /// Home current limit.  This parameter is only used when running
   /// in one of the 'home to hard stop' homing modes.  In all other
   /// modes this parameter may be left uninitialized.
   /// The current should be specified in units of 0.01 Amps
   /// (i.e. 100 for 1.0 Amp)
   int16 current;

   /// Home delay.  This parameter is only used when running
   /// in one of the 'home to hard stop' homing modes.  In all other
   /// modes this parameter may be left uninitialized.
   /// The delay is specified in units of milliseconds.
   int16 delay;

   /// Default constructor, just set the method to none and the other
   /// parameters to zero.
   HomeConfig( void )
   {
      method = CHM_NONE;
      extended = 0;
      velFast = 0;
      velSlow = 0;
      accel   = 0;
      offset  = 0;
      current = 0;
      delay   = 0;
   }
};

/***************************************************************************/
/**
  Amplifier profile parameters.  This structure holds all the parameters 
  related to point-to-point profile moves.
  */
/***************************************************************************/
struct ProfileConfig
{
   /// Type of profile to be used.
   PROFILE_TYPE type;

   /// For absolute moves this is an absolute position,
   /// for relative moves it's a distance to move.
   uunit pos;

   /// Velocity limit for move.
   uunit vel;

   /// Acceleration limit for move.
   uunit acc;

   /// Deceleration limit for move.
   uunit dec;

   /// Acceleration value to use when aborting a running
   /// trajectory.  This is the same as the 'quick stop'
   /// deceleration.
   uunit abort;

   /// Jerk limit for move.
   uunit jrk;

   /// Default constructor.  Simply set all parameters to zero.
   ProfileConfig( void )
   {
      type = PROFILE_TRAP;
      pos = vel = acc = dec = jrk = abort = 0;
   }
};

/***************************************************************************/
/**
  Position and velocity error windows.
  */
/***************************************************************************/
struct TrackingWindows
{
   /// Tracking error window.  
   /// See Amp::SetPositionErrorWindow for more information
   uunit trackErr;

   /// Position warning window.
   /// See Amp::SetPositionWarnWindow for more information
   uunit trackWarn;

   /// Position tracking & settling window.
   /// See Amp::SetSettlingWindow for more information
   uunit settlingWin;

   /// Position tracking & settling time (ms).
   /// See Amp::SetSettlingTime for more info
   uint16 settlingTime;

   /// Velocity warning window
   /// See Amp::SetVelocityWarnWindow for more information
   uunit velWarnWin;

   /// Velocity warning window time
   /// See Amp::SetVelocityWarnTime for more information
   uint16 velWarnTime;

   TrackingWindows( void );
};

/***************************************************************************/
/**
  Motor information structure.  This structure holds information about the 
  motor connected to the amplifier.

  The amplifier uses the information in this structure when controlling the
  motor.  It is very important that the information provided to the amplifier
  be as accurate as possible for proper motor control.

  Use the methods Amp::GetMtrInfo and Amp::SetMtrInfo to upload / download
  the information contained in this structure.

  Note that unlike many amplifier parameters, motor parameters are always 
  stored in non-volatile flash memory.
  */
/***************************************************************************/
struct MtrInfo
{
   /// Motor type
   uint16 type;

   /// Name of the motor manufacturer
   char mfgName[ COPLEY_MAX_STRING ];

   /// Motor model number
   char model[ COPLEY_MAX_STRING ];

   /// Number of pole pairs (i.e. number of electrical phases) 
   /// per rotation.  Used for rotory motors only.
   int16 poles;

   /// Motor resistance (10 milliohm units)
   uint16 resistance;

   /// Motor inductance (10 microhenry units)
   uint16 inductance;

   /// Peak torque (0.01 Newton millimeters)
   uint32 trqPeak;

   /// Continuous torque (0.01 Newton millimeters)
   uint32 trqCont;

   /// Torque constant (0.01 Newton millimeters / Amp)
   uint32 trqConst;

   /// Max velocity 
   uunit  velMax;

   /// Back EMF constant (10 millivolts / KRPM)
   uint32 backEMF;

   /// Inertia
   uint32 inertia;

   /// Motor has a temperature sensor (true/false)
   bool   tempSensor;

   /// Reverse motor wiring if true
   bool   mtrReverse;

   /// Reverse encoder direction if true
   bool   encReverse;

   /// Type of hall sensors on motor.  See documentation for details
   int16 hallType;

   /// Hall wiring code, see documentation for details
   int16 hallWiring;

   /// Hall offset (degrees)
   int16 hallOffset;

   /// Motor has a brake if true
   bool   hasBrake;

   /// Delay (milliseconds) between disabling amp & applying brake
   /// During this time the amp will attempt to actively stop motor
   int16 stopTime;

   /// Delay (milliseconds) between applying brake & disabling PWM
   int16 brakeDelay;

   // Delay (milliseconds) between enabling PWM outputs and releasing the brake
   int16 brakeEnableDelay;

   /// Velocity below which brake will be applied.
   uunit brakeVel;

   /// Encoder type.  See documentation for details
   int16 encType;

   /// Encoder counts / revolution (rotory motors only)
   int32 ctsPerRev;

   /// Encoder units (linear motor only)
   int16 encUnits;

   /// Encoder resolution (encoder units / count) - linear motors only
   int16 encRes;

   /// Motor electrical distance (encoder units / electrical phase) - linear only
   int32 eleDist;

   /// Motor units (used by CME program)
   int16 mtrUnits;

   /// Microsteps / motor rotation (used for Stepnet amplifiers)
   int32 stepsPerRev;

   /// Analog Encoder shift value (used only with Analog encoders)
   int16 encShift;

   /// Index mark distance (reserved for future use)
   int32 ndxDist;

   /// Load encoder type (0 for none).  See amplifier documentation
   /// for possible values.
   int16 loadEncType;

   /// Load encoder resolution.  This is encoder counts/rev for rotory
   /// encoders, or nanometers/count for linear encoders.
   int32 loadEncRes;

   /// Reverse load encoder if true.
   bool  loadEncReverse;

   /// Load encoder gear ratio.  This parameter is used by the CME software
   /// and gives a ratio of motor encoder counts to load encoder counts.
   int32 gearRatio;

   /// Resolver cycles / rev.  This parameter gives the number of 
   /// resolver cycles / motor rev.  It's only used on systems that
   /// use a resolver for position feedback.  Default is 1 cycle/rev.
   uint16 resolverCycles;

   /// Hall velocity shift value.  This parameter is only used on servo systems where
   /// there is no encoder and digital hall sensors are used for velocity feedback.
   /// In that case, this shift value can be used to scale up the calculated velocity.
   int16 hallVelShift;

   /// Motor Encoder options.  This bitmapped parameter is used to specify various
   /// configuration options for the motor encoder.  See documentation.
   uint32 mtrEncOptions;

   /// Load Encoder options.  This bitmapped parameter is used to specify various
   /// configuration options for the load encoder.  See documentation.
   uint32 loadEncOptions;

   // Limit for analog motor temperature sensor. If this param is zero, then the analog
   // motor temp sensor is disabled. If set to a positive value, then a motor temp error 
   // will occur any time the voltage on motor temp input exceeds this value (in mV).
   // If negative, then a motor temp error will occur any time the voltage on the motor
   // temp input is lower than the absolute value of this limit in mV.
   int16 overTempLimit;

   // Open motor wiring current - current applied to check for disconnected motor
   int16 openMtrCrnt;

   // Load encoder calibration settings
   uint32 ldEncCal;

   // Motor encoder calibration settings
   uint32 mtrEncCal;

   // Encoder cosine scaling factor.  Used by resolver & servotube encoder calculations to
   // adjust the cosine signal amplitude to be the same as the sine. If set to zero, both 
   // scaling offsets will be ignored.
   int16 encCosScale;

   // Encoder cosine offset added to signal before calculating position. 
   // Used for resolvers and servotube motors.
   int16 encCosOffset;

   // Encoder sine offset added to signal before calculating position. 
   // Used for resolvers and servotube motors.
   int16 encSinOffset;

   MtrInfo( void );
};

/***************************************************************************/
/**
  Programmable I/O pin configuration
  */
/***************************************************************************/
struct AmpIoCfg
{
   /// Maximum available on any amplifier
#define COPLEY_MAX_INPUTS         26
#define COPLEY_MAX_OUTPUTS        12

   /// Number of programmable inputs available on this amplifier.
   /// This is a read only parameter, writes have no effect.
   uint8 inputCt;

   /// Number of programmable outputs available on this amplifier.
   /// This is a read only parameter, writes have no effect.
   uint8 outputCt;

   /// Input pin pull-up / pull-down resister configuration.
   /// See Amp::SetIoPullup for more information
   uint16 inPullUpCfg;

   /// Input pin pull-up / pull-down resister configuration.
   /// 32 Bit version of paramter inPullUpCfg
   int32 inPullUpCfg32;

   /// Input pin configuration for each pin.
   /// See Amp::SetInputConfig for more information.
   INPUT_PIN_CONFIG inCfg[ COPLEY_MAX_INPUTS ];

   /// Input pin debounce time (milliseconds)
   int16 inDebounce[ COPLEY_MAX_INPUTS ];

   /// Output pin configuration for each output pin
   /// See Amp::SetOutputConfig for more information
   OUTPUT_PIN_CONFIG outCfg[ COPLEY_MAX_OUTPUTS ];

   /// Output pin configuration mask for each pin.
   /// See Amp::SetOutputConfig for more information
   uint32 outMask[ COPLEY_MAX_OUTPUTS ];

    /// Output pin configuration mask for each pin.
   /// See Amp::SetOutputConfig for more information
   uint32 outMask1[ COPLEY_MAX_OUTPUTS ];

   /// IO Options, used to configure the optional features
   /// of the general purpose IO
   int32 ioOptions;

   AmpIoCfg( void );
};

/***************************************************************************/
/**
  Software limit switch configuration.  This structure may be used to pass
  software limit switch settings to an Amp object using the functions
  Amp::SetSoftLimits and Amp::GetSoftLimits
  */
/***************************************************************************/
struct SoftPosLimit
{
   /// Negative limit position.  Any time the motors actual position is less
   /// then this value, a negative software limit condition will be in effect
   /// on the amplifier.
   uunit neg;

   /// Positive limit position.  Any time the motors actual position is greater
   /// then this value, a positive software limit condition will be in effect
   /// on the amplifier.
   uunit pos;

   /// Software limit acceleration.
   /// 
   /// This parameter defines the acceleration value that will be used to stop the 
   /// motor at the software limit position.  Note that this parameter was added in
   /// amplifier firmware version 4.60.  Before that version the older current based
   /// software limit processing was used.
   ///
   /// If this parameter is set to zero (the default) then the software limits will
   /// act like virtual limit switches.  If the motor position exceeds the limit position
   /// then the amplifier will refuse to output current in the limit direction.
   uunit accel;

   /// Motor position wrap value
   ///
   /// Actual motor position will wrap back to zero when this value is reached.
   /// Setting this value to zero disables thsi feature. (Units counts).
   /// This feature is only implemented on the 8367 hardware with fimware 0.42
   /// or later
   uunit motorPosWrap;

   /// Load postion wrap. Same as above except for load encoder.
   uunit loadPosWrap;

   /// MACRO encoder caputure configuration. Configures the MACRO amplifier's encoder
   /// caputure circuit. Only implemented on MACRO amplifiers.
   uunit macroEncoderCapture;

   /// Default constructor.  Simply sets both limits to zero.
   SoftPosLimit( void )
   {
      neg = pos = 0;
      accel = 0;
      motorPosWrap = loadPosWrap = 0;
   }
};

/***************************************************************************/
/**
  Configuration structure used to set up the amplifier regeneration resister.
  The regen resister is not available on all amplifier models (currently only
  on the Xenus offline amplifier).

  These settings may be up/download from the amplifier using the functions
  Amp::SetRegenConfig and Amp::GetRegenConfig.
  */
/***************************************************************************/
struct RegenConfig
{
   /// Model number / name string for regen resister connected 
   /// to the amplifier.
   char model[ COPLEY_MAX_STRING ];

   /// Regen resister resistance (100 milliohm units)
   uint16 resistance;

   /// Continuous power limit for regen resister (Watts).  This is
   /// the amount of power that the resister is able to disapate
   /// continuously
   uint16 contPower;

   /// Peak power limit for resister (Watts).  This is the maximum
   /// amount of power that the resister is able to dissapate for
   /// a limited amount of time.
   uint16 peakPower;

   /// Peak time limit (milliseconds).  This is the amount of time
   /// that the regen resister is able to dissapate peak power before
   /// it needs to be folded back to the continuous power limit.
   uint16 peakTime;

   /// Regen resister turn on voltage (100 millivolt units).  When the bus voltage 
   /// rises above this value the regen resister will be enabled.
   uint16 vOn;

   /// Regen resister turn off voltage (100 millivolt units).  When the bus voltage
   /// drops below this value, the regen resiter will be disabled.
   uint16 vOff;

   /// Default constructor.
   /// Initializes all structure elements to zero.
   RegenConfig( void )
   {
      model[0] = 0;
      resistance = 0;
      contPower = 0;
      peakPower = 0;
      peakTime = 0;
      vOn  = 0;
      vOff = 0;
   }
};

/***************************************************************************/
/**
  Configuration parameters for amplifier's internal function generator.
  */
/***************************************************************************/
struct FuncGenConfig
{
   /// Configuration
   int16 cfg;

   /// Duty cycle in 0.1% (i.e. 0 to 1000)
   int16 duty;

   /// Frequency (Hz)
   int16 freq;

   /// Amplitude.  Units depend on what the function generator is driving
   /// 0.01 Amps for current.
   /// 0.1 encoder counts/sec for veloctiy.
   /// Encoder counts for position.
   int32 amp;

   /// Default constructor, sets all members to zero
   FuncGenConfig( void )
   {
      cfg = duty = freq = 0;
      amp = 0;
   }
};

/***************************************************************************/
/**
  Analog input configuration.  These parameters are used when the amplifier 
  is being driven from it's analog reference input pin.  Note that not all
  amplifier have an analog input reference, and that this is not a standard
  CANopen mode of operation.
  */
/***************************************************************************/
struct AnalogRefConfig
{
   /// Calibration offset.  This offset is set at the factory and
   /// should normally not be modified.  Units are millivolts
   int16 calibration;

   /// Offset in millivolts.
   int16 offset;

   /// Deadband in millivolts.  The analog input will be treated as
   /// zero when it's absolute value is less then this.
   int16 deadband;

   /// Scaling factor.  Units are dependent on the mode of operation:
   /// 0.01 Amp when driving current.
   /// 0.1 Encoder counts/second when driving velocity
   int32 scale;

   /// Default constructor.  Simply sets all members to zero.
   AnalogRefConfig( void )
   {
      calibration = 0;
      offset      = 0;
      deadband    = 0;
      scale       = 0;
   }
};

/***************************************************************************/
/**
  PWM or Pulse/Direction input configuration.  These parameters are used when
  the amplifier is being controlled through it's PWM inputs (current or veloctiy
  mode), or pulse/direction input pins (position mode).  These parameters have
  no effect when running in standard CANopen modes of operation.
  */
/***************************************************************************/
struct PwmInConfig
{
   /// PWM input pin configuration.  See amplifier documentation for
   /// detailed information.
   int16 cfg;

   /// PWM input UV configuration. Used to configure the drive when
   /// running in UV mode (desired state 5). See documentation
   int32 uvCfg;

   /// Scaling factor.  Units are dependent on the mode of operation:
   /// 0.01 Amp when driving current.
   /// 0.1 Encoder counts/second when driving velocity
   /// Encoder counts (upper 16 bits) / pulses (lower 16 bits)
   /// ratio for position mode.
   int32 scale;

   /// PWM input frequency.  This parameter is only used when running in 
   /// UV current mode.  For other PWM or step/dir input modes the PWM
   /// frequency is automatically calculated by the amplifier and this
   /// parameter is ignored.
   /// The frequency is set 10 Hz units.  For example, setting this parameter
   /// to 100 indicates that the PWM input frequency is 1kHz.
   int16 freq;

   /// PWM input deadband. This parameter was added to plus products starting
   /// with version 2.75. Previously (and on older products), 0x2322 was used
   /// as a deadband value.
   /// The range of 0 to 32k equals deadband of 0% to 100%.
   int16 deadBand;

   // Minimum PWM pulse width in microseconds. Used when running in PWM position mode.
   // In this mode the PWM input pulse width is captured by the drive and used to 
   // calculate an absolute position using the formula outlined by CAN ID 0x2323
   int16 minPulseWidth;

   // Maximum PWM pulse width in microseconds, used when running in PWM position mode.
   int16 maxPulseWidth;

   /// Default constructor.  Simply sets all members to zero.
   PwmInConfig( void )
   {
      cfg   = 0;
      uvCfg = 0;
      scale = 0;
      freq  = 1000;
	  deadBand = 0;
      minPulseWidth = 0;
      maxPulseWidth = 0;
   }
};

/***************************************************************************/
/**
  CANopen network bit rate enumeration.
  */
/***************************************************************************/
enum CAN_BIT_RATE
{
   CAN_RATE_1MEG = 0x0000,  ///< 1,000,000 bits / second
   CAN_RATE_800K = 0x1000,  ///<   800,000 bits / second
   CAN_RATE_500K = 0x2000,  ///<   500,000 bits / second
   CAN_RATE_250K = 0x3000,  ///<   250,000 bits / second
   CAN_RATE_125K = 0x4000,  ///<   125,000 bits / second
   CAN_RATE_50K  = 0x5000,  ///<    50,000 bits / second
   CAN_RATE_20K  = 0x6000,  ///<    20,000 bits / second
   CAN_RATE_100K = 0x8000   ///<   100,000 bits / second
};

/***************************************************************************/
/**
  CANopen Node ID and bit rate configuration.  The amplifier's CANopen node ID 
  and network bit rate can be configured using the members of this structure.  
  Note that the ID and bit rate are only set on power-up or reset, so after 
  programming a new configuration the amplifier must be reset for the 
  configuration to become active.

  The CANopen node ID is a 7 bit number in the range 1 to 128.  The value 0 is
  reserved and is not considered a valid node ID.  Selecting a node ID of 0
  will cause the amplifier to stop communicating over the CANopen network.

  The node ID is calculated using a combination of any of the following:
  - The CAN address switch on amplifiers which support this feature
  - 0 to 7 general purpose input pins.
  - A programmable offset in the range 0 to 128.

  On startup, the input pins are read first.  The inputs that will be used for
  CAN address selection are the highest numbered pins available.  For example,
  on an amplifier with 12 input pins (0 to 11), if 3 inputs are used for CANopen
  node ID selection, then the pins used will be 9, 10 and 11.  These three pins
  will result in a base node address between 0 and 7.

  If the CAN address switch is being used, then the value read from the input 
  pins will be shifted up four bits (multiplied by 16) and the value of the 
  input switch will be added to it.

  Finally, the programmed offset will be added to the value read from the input
  pins and address switch.  The lowest 7 bits of this sum will be used as the 
  CANopen node ID.  If this value results in an ID of zero, the CANopen interface
  will be disabled.

  For example, to program an amplifier to ignore input pins and the address
  switch, and just set a fixed ID of 7, set the number of input pins to zero, 
  turn off the address switch, and set the offset to 7.
  */
/***************************************************************************/
struct CanNetworkConfig
{
   /// Number of general purpose input pins to read on startup for 
   /// node ID selection.  This parameter may be set from 0 to 7.
   uint8 numInPins;

   /// If true, use the CAN address switch as part of the node ID 
   /// calculation.  Note that on amplifiers which do not support this
   /// switch this parameter is ignored.
   bool useSwitch;

   /// Offset added to the value read from the input pins & address
   /// switch.
   uint8 offset;

   /// CANopen network bit rate to use.
   CAN_BIT_RATE bitRate;

   /// Input pin mapping for node ID selection.  When network node id indicates
   /// that 1 or more input pins will be used to select the node ID, this
   /// parameter is used to map input pins to ID bits.  Availabel after 3.35
   /// firmware.  See documentation
   uint32 pinMapping;

   /// CANopen quick stop option code
   uint16 quickStop;

   /// CANopen shutdown option code
   uint16 shutDownOption;

   /// CANopen disable option code
   uint16 disableOption;

   /// CANopen halt option code
  uint16 haltOption;

   /// CANopen heartbeat time.  The frequency at which the amp will produce
   /// heartbeat messages.  This parameter may be set to zero to disable
   /// heartbeat production.
  uint16 heartbeat;

   /// CANopen Node Guard time.  This parameter gives the time between
   /// nodeguarding requests that are sent form the CANopen master to 
   /// this amplifier
  uint16 nodeGuard;

   /// CANopen Node Guarding Life time factor. This objct gives a multiple
   /// of the CANopen Node Guarding Time
  uint16 nodeGuardLife;

   /// Default constructor.  Set default values.
   CanNetworkConfig()
   {
      numInPins = 0;
      useSwitch = false;
      offset = 0;
      bitRate = CAN_RATE_1MEG;
      pinMapping = 0;
      quickStop = 2; 
      shutDownOption = 0;
      disableOption = 1;
      haltOption = 1;
      heartbeat = 0;
      nodeGuard = 0;
      nodeGuardLife = 3;
   }

   void FromAmpFormat( uint16 a );
   uint16 ToAmpFormat( void );
};

/***************************************************************************/
/**
  Configuration structure used to configure the amplifiers network support.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetNetworkOptions and Amp::GetNetworkOptions.
  */
/***************************************************************************/
struct NetworkOptions
{
   /// Network Options. The details fo this parameter depend on the type
   // of network implemented in the amplifier. See CAN ID 0x21B3.
   uint16 canBusConfig;

   /// Default constructor.
   /// Initialize all structure elements to zero.
   NetworkOptions( void )
   {
      canBusConfig = 0;
   }
}; 

/***************************************************************************/
/**
  Configuration structure used to hold the settings for a drive's D/A converter.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetDAConverterConfig and Amp::GetDAConverterConfig
  */
/***************************************************************************/
struct DAConfig
{
   /// D/A converter configuration. This paramter sets the mode for the D/A
   /// converter on drives so equipped. See CAN ID 0x21E0 for details.
   int32 daConverterConfig;

   /// Default constructor
   /// Initialize all elements to zero
   DAConfig( void )
   {
      daConverterConfig = 0;
   }
};

/***************************************************************************/
/**
  Configuration structure used to set up the microstepper.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetUstepConfig and Amp::GetUstepConfig.
  */
/***************************************************************************/
struct UstepConfig
{
   /// Maximum Velocity adjustment. This is the maximum velocity adjustment made 
   /// by the stepper outer position loop when enbled.  This parameter is only 
   /// used when the stepper outer loop ios engaged. ( when bit 1 of ustepConfig 
   /// ustepConfigAndStatus is set)  
   uint32 maxVelAdj;

   /// Propertional Gain for stepper outer loop.  This parameter gives the gain used
   /// for calculating a velocity adjustment based on position error.  This parameter
   /// is only used when stepper outer loop is engaged ( when bit 1 of ustepConfig 
   /// ustepConfigAndStatus is set)
   uint16 ustepPGainOutLoop;

   /// Detent correction Gain factor
   int16 detentCorrectionGain;


   /// Stepper config and status Bit mapped as follows 
   /// (Bit 0  Use the encoder input for phase compensation if enabled.  Pure stepper if disabled
   /// (Bit 1  Use encoder outer loop to adjust the stepper position based on position error.
   ///         When this bit is set, the gain value maximum velocity adjustment is multiplied
   ///         by the position error, and the result is a velocity that is added
   ///         to the microstepping position
   /// bits 2 - 15 reserved
   uint16 ustepConfigAndStatus;

   /// Default constructor.
   /// Initializes all structure elements to zero.
   UstepConfig( void )
   {
      maxVelAdj = 0;
      ustepPGainOutLoop = 0;
      detentCorrectionGain = 0;
      ustepConfigAndStatus = 0;
   }
};

/***************************************************************************/
/**
  Configuration structure used to set up algorithmic phase init.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetAlgoPhaseInit and Amp::GetAlgoPhaseInit.
  */
/***************************************************************************/
struct AlgoPhaseInit
{
  
   /// Maximum Current to use with algorithmic phase initialization. (0.01 amp units) 
   uint16 phaseInitCurrent;

   /// Algorithmic phase initialization timeout. (milliseconds) 
   uint16 phaseInitTime;

   /// Algorithmic Phase Initialization config.  Bit mapped as follows 
   /// (Bit 0  If clear, use algorithmic phase initialization.
   ///         If set force the phase angle to 0.
   /// bits 1 - 15 reserved
   uint16 phaseInitConfig;

   /// Default constructor.
   /// Initializes all structure elements to zero.
   AlgoPhaseInit( void )
   {
      phaseInitCurrent = 0;
      phaseInitTime = 0;
      phaseInitConfig = 0;
   }
};

/***************************************************************************/
/**
  Configuration structure used to set up the camming.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetCammingConfig and Amp::GetCammingConfig.
  */
/***************************************************************************/
struct CammingConfig
{
   /// Camming Mode configuration.  See documentation for details
   uint16 cammingModeConfig;

   /// Camming delay forward.(Units:master command counts)
   uint16 cammingDelayForward;

   /// Camming delay forward.(Units:master command counts)
   uint16 cammingDelayReverse;


   /// Camming master velocity. Constnat velocity of the Camming
   /// internal generator. (Units 0.1 counts/s)
   int32 cammingMasterVel;

   /// Default constructor.
   /// Initializes all structure elements to zero.
   CammingConfig( void )
   {
      cammingModeConfig = 0;
      cammingDelayForward = 0;
      cammingDelayReverse = 0;
      cammingMasterVel = 0;
   }
};

/***************************************************************************/
/**
  Configuration structure used to set up the Gain Scheduling.
  
  These settings may be up/download from the amplifier using the functions
  Amp::SetGainScheduling and Amp::GetGainScheduling.
  */
/***************************************************************************/
struct GainScheduling
{
   /// Gain Scheduling configuration.  See documentation for details
   uint32 gainSchedulingConfig;

   /// Default constructor.
   /// Initializes all structure elements to zero.
   GainScheduling( void )
   {
      gainSchedulingConfig = 0;
   }
};

/***************************************************************************/
/**
  Amplifier configuration structure.  This structure contains all user 
  configurable parameters used by an amplifier which may be stored in non-volatile
  memory.
  */
/***************************************************************************/
struct AmpConfig
{
   /// Amplifier axis name
   char name[ COPLEY_MAX_STRING ];

   /// Amplifier default mode of operation.
   AMP_MODE controlMode;

   /// Amplifier phasing mode.  This parameter controls the type of
   /// commutation used by the amplifer.
   AMP_PHASE_MODE phaseMode;

   /// PWM output mode.  This parameter can be used to configure the
   /// pwm output section of the amplifier.
   AMP_PWM_MODE pwmMode;

   /// String used by CME to save state information
   /// This string is reserved and should not be modified.
   char CME_Config[ COPLEY_MAX_STRING ];

   /// Amplifier fault mask register
   AMP_FAULT faultMask;

   /// Programmed veloctiy value.  This parameter is only used
   /// in Amp mode AMPMODE_PROG_VEL.  
   uunit progVel;

   /// Programmed current value (0.01 amp units).  This parameter
   /// is only used in Amp mode AMPMODE_PROG_CRNT.
   int16 progCrnt;

   /// Amplifier options.  This parameter is reserved for
   /// future use.
   uint32 options;

   /// Diagnostic microstepping rate.  This parameter gives the
   /// microstep rate (degrees / second) for use in a special 
   /// diagnostic microstepping mode.  The parameter is not 
   /// used in normal CANopen modes, and is only include here
   /// for completness.
   int16 stepRate;

   /// Index capture configuration.  This parameter is not normally
   /// used in CANopen mode and is included here for completness only.
   uint16 capCtrl;

   /// Trajectory abort jerk
   uint32 trjJrkAbort;

   /// One bit of a standard CANopen status word is user
   /// programmable using this setting.  This feature is not
   /// used by CML and is only included here for completness.
   EVENT_STATUS limitBitMask;

   /// 

   /// Some amplifier models provide a secondary encoder connected
   /// which can be configured as either an input or output.  This
   /// parameter is used to configure this hardware.
   uint16 encoderOutCfg;

   /// CANopen network configuration
   CanNetworkConfig can;

   /// Position loop configuration
   PosLoopConfig    pLoop;

   /// Velocity loop configuration
   VelLoopConfig    vLoop;

   /// Current loop configuration
   CrntLoopConfig   cLoop;

   /// Motor information
   MtrInfo          motor;

   /// Tracking window settings
   TrackingWindows  window;

   /// Software position limits
   SoftPosLimit     limit;

   /// General purpose I/O pin configuration
   AmpIoCfg         io;

   /// Homing mode configuration
   HomeConfig       home;

   /// Trajectory profile settings
   ProfileConfig    profile;

   /// Analog reference input settings
   AnalogRefConfig  ref;

   /// PWM input configuration
   PwmInConfig      pwmIn;

   /// Internal function generator settings
   FuncGenConfig    fgen;

   /// Regeneration resister configuration
   RegenConfig      regen;

   /// Velocity loop output filter settings
   Filter           vloopOutFltr;

   /// Velocity loop command filter settings
   Filter           vloopCmdFltr;

   // Second chained biqaud filter on output of veloicty loop
   Filter           vloopOutFltr2;

   // Third chained biqaud filter on output of velocity loop
   Filter           vloopOutFltr3;

   // First chained biquad filter on input of current loop
   Filter           iloopCmdFltr;

   // Second chained biquad filter on input of current loop
   Filter           iloopCmdFltr2;

   /// Analog command filter settings
   Filter           aInCmdFltr;

   /// Input shaping filter
   InputShaper      inputShaping;    

   /// Stepper configuration
   UstepConfig      ustep;

   /// Algorithmic Phase Initialization
   AlgoPhaseInit    algoPhaseInit;

   /// Camming Configuration
   CammingConfig    camming;
   
   /// Gain Scheduling Configuration
   GainScheduling   gainSched;

   /// Can Bus network support
   NetworkOptions   netOptions;

   /// D/A converter configuration
   DAConfig         daConfig;

   /// Configure various parts of the amps servo loops
   ServoLoopConfig  servoConfig;

   /// Encoder error filter configuration.
   EncoderErrorConfig errFilterCfg;

   /// Default constructor.  Simply sets all members to zero.
   AmpConfig()
   {
      name[0] = 0;
      controlMode = AMPMODE_DISABLED;
      phaseMode = PHASE_MODE_ENCODER;
      pwmMode = PWM_MODE_STANDARD;
      CME_Config[0] = 0;
      faultMask = (AMP_FAULT)0;
      progVel = 0;
      progCrnt = 0;
      options = 0;
      stepRate = 0;
      capCtrl = 0;
	  trjJrkAbort = 0;
      limitBitMask = (EVENT_STATUS)0;
      encoderOutCfg = 0;
   }
};

CML_NAMESPACE_END()

#endif

