/* XSens Sensor class implementation
 *
 */

#include "xsens.h"

using namespace std;

float XSensSensor::pitch;
float XSensSensor::roll;
float XSensSensor::pitchSamples[3] = {0, 0, 0};
float XSensSensor::rollSamples[3] = {0, 0, 0};
MotorController *XSensSensor::mc;
//std::fstream XSensSensor::fs_xsens;
float XSensSensor::logData[2] = {0,0};
int 	XSensSensor::logNum = 2;
bool  XSensSensor::needToCycleLogger = false;
Logger *XSensSensor::xs_logger;

// Constructor
XSensSensor::XSensSensor() {


	LOG(4, "Creating XsControl object..." << endl);
	control = XsControl::construct();
	assert(control != 0);

	// Lambda functions for error handling --> TODO: determine if this is still needed
	auto handleError = [=](string errorString) {
		control->destruct();
		LOG(1, errorString << endl);
		//LOG(1, "Press [ENTER] to continue." << endl);
		//cin.get();
		throw "Error making XSens object";
	};

	LOG(4, "Scanning for devices..." << endl);
	XsPortInfoArray portInfoArray = XsScanner::scanPorts();

	// Find an MTi device
	XsPortInfo mtPort;
	for (auto const &portInfo : portInfoArray) {
		
		if (portInfo.deviceId().isMti() || portInfo.deviceId().isMtig()) {
			mtPort = portInfo;
			break;
		}
	}

	if (mtPort.empty())
		handleError("No MTi device found. Aborting.");

	LOG(4, "Found a device with device ID: " << mtPort.deviceId().toString().toStdString() << " @ port: " << mtPort.portName().toStdString() << ", baudrate: " << mtPort.baudrate() << endl);

	LOG(4, "Opening port..." << endl);
	if (!control->openPort(mtPort.portName().toStdString(), mtPort.baudrate()))
		handleError("Could not open port. Aborting.");

	// Get the device object
	device = control->device(mtPort.deviceId());
	assert(device != 0);

	LOG(4, "Device: " << device->productCode().toStdString() << ", with ID: " << device->deviceId().toString() << " opened." << endl);

	// Create and attach callback handler to device to process new packets
	callback = new CallbackHandler(XSensSensor::packet_parser);
	device->addCallbackHandler(callback);
	
	// Put the device in configuration mode before configuring the device
	LOG(4, "Putting device into configuration mode..." << endl);
	if (!device->gotoConfig())
		handleError("Could not put device into configuration mode. Aborting.");

	LOG(4, "Configuring the device..." << endl);

	// TODO: Determine if mtb file is needed, otherwise delete this:
	device->readEmtsAndDeviceConfiguration();

	configArray = XsOutputConfigurationArray();
	configArray.push_back(XsOutputConfiguration(XDI_PacketCounter, 0));
	configArray.push_back(XsOutputConfiguration(XDI_SampleTimeFine, 0));

	// This Config setting will depend on the MTi model --> this is for the MTi-630
	configArray.push_back(XsOutputConfiguration(XDI_Quaternion, 0));
	
	if (!device->setOutputConfiguration(configArray))
		handleError("Could not configure MTi device. Aborting.");

	/*
	// TODO: Determine if this is still needed
	LOG(4, "Creating a log file..." << endl);
	string logFileName = "logfile.mtb";
	if (device->createLogFile(logFileName) != XRV_OK)
		handleError("Failed to create a log file. Aborting.");
	else
		LOG(4, "Created a log file: " << logFileName.c_str() << endl);

	*/
	LOG(4, "Putting device into measurement mode..." << endl);
	if (!device->gotoMeasurement())
		handleError("Could not put device into measurement mode. Aborting.");
	
	// TODO: Determine if I should start recording here

	LOG(4, "Starting recording..." << endl);
	if (!device->startRecording())
		handleError("Failed to start recording. Aborting.");

	//fs_xsens.open(XSENS_LOG_FILE, std::fstream::out);
	xs_logger = new Logger(XSENS_LOG_DIR);
	needToCycleLogger = false;

	this->mc = NULL;
}

XSensSensor::XSensSensor(MotorController &mc) {

	this->mc = &mc;

	LOG(4, "Creating XsControl object..." << endl);
	control = XsControl::construct();
	assert(control != 0);

	// Lambda functions for error handling --> TODO: determine if this is still needed
	auto handleError = [=](string errorString) {
		control->destruct();
		LOG(1, errorString << endl);
		//LOG(1, "Press [ENTER] to continue." << endl);
		//cin.get();
		throw "Error making XSens object";
	};

	LOG(4, "Scanning for devices..." << endl);
	XsPortInfoArray portInfoArray = XsScanner::scanPorts();

	// Find an MTi device
	XsPortInfo mtPort;
	for (auto const &portInfo : portInfoArray) {
		
		if (portInfo.deviceId().isMti() || portInfo.deviceId().isMtig()) {
			mtPort = portInfo;
			break;
		}
	}

	if (mtPort.empty())
		handleError("No MTi device found. Aborting.");

	LOG(4, "Found a device with device ID: " << mtPort.deviceId().toString().toStdString() << " @ port: " << mtPort.portName().toStdString() << ", baudrate: " << mtPort.baudrate() << endl);

	LOG(4, "Opening port..." << endl);
	if (!control->openPort(mtPort.portName().toStdString(), mtPort.baudrate()))
		handleError("Could not open port. Aborting.");

	// Get the device object
	device = control->device(mtPort.deviceId());
	assert(device != 0);

	LOG(4, "Device: " << device->productCode().toStdString() << ", with ID: " << device->deviceId().toString() << " opened." << endl);

	// Create and attach callback handler to device to process new packets
	callback = new CallbackHandler(XSensSensor::packet_parser);
	device->addCallbackHandler(callback);
	
	// Put the device in configuration mode before configuring the device
	LOG(4, "Putting device into configuration mode..." << endl);
	if (!device->gotoConfig())
		handleError("Could not put device into configuration mode. Aborting.");

	LOG(4, "Configuring the device..." << endl);

	// TODO: Determine if mtb file is needed, otherwise delete this:
	device->readEmtsAndDeviceConfiguration();

	configArray = XsOutputConfigurationArray();
	configArray.push_back(XsOutputConfiguration(XDI_PacketCounter, XSENS_OUTPUT_FREQUENCY));
	configArray.push_back(XsOutputConfiguration(XDI_SampleTimeFine, XSENS_OUTPUT_FREQUENCY));

	// This Config setting will depend on the MTi model --> this is for the MTi-630
	configArray.push_back(XsOutputConfiguration(XDI_Quaternion, XSENS_OUTPUT_FREQUENCY));
	
	if (!device->setOutputConfiguration(configArray))
		handleError("Could not configure MTi device. Aborting.");

	/*
	// TODO: Determine if this is still needed
	LOG(4, "Creating a log file..." << endl);
	string logFileName = "logfile.mtb";
	if (device->createLogFile(logFileName) != XRV_OK)
		handleError("Failed to create a log file. Aborting.");
	else
		LOG(4, "Created a log file: " << logFileName.c_str() << endl);

	*/
	LOG(4, "Putting device into measurement mode..." << endl);
	if (!device->gotoMeasurement())
		handleError("Could not put device into measurement mode. Aborting.");
	
	// TODO: Determine if I should start recording here

	LOG(4, "Starting recording..." << endl);
	if (!device->startRecording())
		handleError("Failed to start recording. Aborting.");

	//fs_xsens.open(XSENS_LOG_FILE, std::fstream::out);
	xs_logger = new Logger(XSENS_LOG_DIR);
	needToCycleLogger = false;

}
// Destructor
XSensSensor::~XSensSensor() {
	/*
	LOG(4, "Closing log file..." << endl);
	if (!device->closeLogFile())
		LOG(1, "Failed to close log file.");
		
		*/
	
	LOG(4, "Closing port..." << endl);
	control->closePort(mtPort.portName().toStdString());
	
	LOG(4, "Freeing XsControl object..." << endl);
	control->destruct();

	delete callback;
	delete xs_logger;
	//fs_xsens.close();

	LOG(4, "Successful deletion of XSensSensor." << endl);

}

void XSensSensor::packet_parser(const XsDataPacket *packet) {

	// Commented out sections can be used for other XSens sensors (e.g. MTi-670)
	/*
	if (packet.containsCalibratedData()) {
		XsVector acc = packet.calibratedAcceleration();
		XsVector gyr = packet.calibratedGyroscopeData();
		XsVector mag = packet.calibratedMagneticField();
	}
	*/
	XsQuaternion quaternion;
	XsEuler euler;

	if (packet->containsOrientation()) {
		quaternion 	= packet->orientationQuaternion();
		euler 		= packet->orientationEuler();

		pitch 		= euler.pitch();
		roll 			= euler.roll();
	}

	/**** New sensor mounting adjustments ****/
	int multiplier  = roll > 0 ? -1 : 1;

	roll = 180 - std::abs(roll);

	roll *= multiplier;


	/***** MANUAL OFFSET ******/
	roll += 10;


	/*
	if (packet.containsLatitudeLongitude())
		XsVector latLon = packet.latitudeLongitude();
	
	if (packet.containsAltitude())
		float alt = packet.altitude();
	
	if (packet.containsVelocity())
		XsVector vel = packet.velocity(XDI_CoordSysEnu);
	*/

	// Use data to update motor velocities
	if (mc)
		mc->correctInclination(pitch, roll);

	// Log current inclination and time
	//fs_xsens << pitchSamples[0] << "," << pitchSamples[1] << "," << pitchSamples[2] << "," << avgPitch << ",";
	//fs_xsens << rollSamples[0] << "," << rollSamples[1] << "," << rollSamples[2] << "," << avgRoll << std::endl;

	//fs_xsens << pitch << "," << roll << std::endl; 
	
	// TODO: new logging
	logData[0] = pitch;
	logData[1] = roll;

	xs_logger->log(logData, logNum);

	if (needToCycleLogger) {
		xs_logger->cycleLogFile();
		needToCycleLogger = false;
	}

}


// Access methods
/*
XSSM* XSensSensor::get_shared_memory() {
	return this->shared_memory;
}
*/


float XSensSensor::getPitch() {
	return this->pitch;
}


float XSensSensor::getRoll() {
	return this->roll;
}

void XSensSensor::cycleLogger() {
	needToCycleLogger = true;
}

