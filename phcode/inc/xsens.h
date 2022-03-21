#ifndef XSENSSENSOR_H
#define XSENSSENSOR_H

/* XSens Sensor Class Header
 *	
 * Used to request and handle packets of information from the XSens sensor
 * (MTi-630) 
 *
 * This class was adpated from the XSens example_mti_receive_data.cpp file 
 */

#include <xscontroller/xscontrol_def.h>
#include <xscontroller/xsdevice_def.h>
#include <xscontroller/xsscanner.h>
#include <xstypes/xsoutputconfigurationarray.h>
#include <xstypes/xsdatapacket.h>
#include <xstypes/xstime.h>
#include <xscommon/xsens_mutex.h>

#include <iostream>
#include <iomanip>
#include <list>
#include <string>
#include <fstream>

#include "callback_handler.h"
#include "shared_memory.h"
#include "movingavg.h"
#include "logger.h"

//#include "dynamic_functions.h"


/* Hardware parameters for Vectornav -- TODO: See if still relevant for XSens */
#define XSENS_OUTPUT_FREQUENCY 25
#define XSENS_PORT "/dev/ttyUSB0"
#define XSENS_BAUDRATE 115200

#define XSENS_LOG_FILE "/home/pi/xsens_log.csv"
#define XSENS_LOG_DIR "/home/pi/PH_XSens_Logs/"


using namespace std;

template<typename... Args>
struct pack { };

/* Data packet parameters -- copied over from VectorNav sensor TODO: Adapt this for XSens paramters */
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
  uint64_t, // gps time 	<0>
  float, // yaw 				<1>
  float, // pitch 			<2>
  float, // roll 				<3>
  float, // quaternion w 	<4>
  float, // quaternion x 	<5>
  float, // quaternion y 	<6>
  float  // quaternion z 	<7>
> XSSM;

#include "motorcontroller.h"


class XSensSensor {
	public:
		XSensSensor();
		XSensSensor(MotorController &mc);
		~XSensSensor();
		//XSSM *get_shared_memory();
		static void packet_parser(const XsDataPacket *);
		float getPitch();
		float getRoll();
		static float pitch;
		static float roll;
		static float logData[2];
		static int logNum;
		void cycleLogger();
		//static std::fstream fs_xsens;
	private:
		//XSSM *shared_memory;
		XsControl* control;
		XsPortInfoArray portInfoArray;
		XsPortInfo mtPort;
		CallbackHandler* callback;
		XsOutputConfigurationArray configArray;
		XsDevice* device;
		static float pitchSamples[3];
		static float rollSamples[3];
		static MotorController *mc;

		// Dynamic Functions --> uncomment at end
		/*
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
		*/

	   const std::string xs_port = XSENS_PORT;
	   const uint32_t xs_baudrate = XSENS_BAUDRATE;

		static Logger *xs_logger;
		static bool needToCycleLogger;
};

#endif
