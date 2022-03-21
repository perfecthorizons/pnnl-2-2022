#include "motorcontroller.h"
#include "literals.h"
#include "debug.h"
#include "alerts.h"
#include "stdio.h"
#include <iostream>
#include <thread>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "xsens.h"

using namespace std;

/* Globals */

// XSensSensor globals
constexpr static const char *xs_data_path="/home/pi/XSensData";
char XS_fn[50]; // Not sure what this is for
stringstream XS_filename;

static XSensSensor *xs = NULL;
Journaller *gJournal = 0; // This is needed for XSens sensor



/********************************************** TODO: see if this can be removed */
// E-stop globals
static const int ESTOP_PIN = 1;
static volatile bool estopPressed = false;
static char ipAddress[15] = "0.0.0.0";


#ifdef PRODUCTION
  #include <unistd.h>
  #include <signal.h>
  #include <cerrno>

//  #include "pid_file.h"
static volatile bool terminate_flag = false;
static bool brakeStatus1, brakeStatus2, brakeStatus = false;
static bool motorFaultStatus1, motorFaultStatus2, motorFaultStatus = false;

//  static float pitch, roll;

void interrupt_handler(int s) {
 terminate_flag = true;
}


/**
 * Signal handler which is called when the log file should be reset; 
 * closes the existing log file and creates a new, timestamped one
 */
void log_cycle_handler(int s) {
	std::cout << "Received SIGUSR1" << std::endl;
	if (xs) {
		xs->cycleLogger();
	}
}

#endif

/**********************************************************************************/


/**** Function Prototypes ****/

// Menu Print Functions
void printSeparator();
void printEstopStatus();
void printIP();
void printMotorFaults();
void printXSStatus();
void printBrakeStatus();
void printSharedMemStatus(); // TODO: remove
void printXSData();
void printMenu();
void printAmpParameters(MotorController *mc); // TODO: move to shell

// Status Check Functions
bool getEstopStatus();
bool getBrakeStatusMotor(uint16_t ampID);
void getBrakeStatus();
bool getMotorFaultStatuses();

// Utility Functions
char* getIPAddress(const char* adapter);
void setBrake(bool brakeOn);
void clearMotorFaults();
void manualMove(uint16_t ampID); // TODO: convert to "shell" function


void tuningMenu(MotorController *mc, Alerts *alerts);

// Other Function TODO sort through
void ServiceMode();
void __gain_tuning_shell(MotorController* mc, Alerts *alerts);



/**** Main Implementation ****/

int main(int argc, char *argv[]) {

  #ifdef PRODUCTION
    //if (daemon(1, 0) != 0) {
    //  LOG(0, "Error daemonizing process. Error Number " << errno << ".\n");
    //  std::exit(1);
    //}
    //PIDFile p("/tmp/casper2.pid");

    signal(SIGINT, interrupt_handler);
	 signal(SIGUSR1, log_cycle_handler);

  #endif
  
  wiringPiSetup();
  pinMode(ESTOP_PIN, INPUT);
  pullUpDnControl(ESTOP_PIN, PUD_DOWN);


  Alerts *alerts;
  try {
    alerts = new Alerts();
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  }

  alerts->diagnostic("Stabilization Platform coming online.");


  /* XSens Integration begins here */
/*
	cout << "Creating XSSM...";
	XSSM *xssm = xs->get_shared_memory();
	cout << "done.\n";

*/
   /*
	cout << "Creating data_logger...";
	time_t ttt = time(nullptr);
	tm tmm = *localtime(&ttt);
	XS_filename << xs_data_path << put_time(&tmm, "%Y_%m_%d_%T.txt");
	XS_filename >> XS_fn;
	data_logger<uint64_t, float, float, float, float, float, float, float> xs_logger(xssm, xs_data_path, 20_hz);
	cout << "done.\n";
	*/

  /* --------------------- */
  
  //cml.SetDebugLevel(LOG_EVERYTHING);
 
  MotorController *mc;
  try {
    mc = new MotorController(alerts);
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  };

   cout << "Creating XSensSensor...";
  	try {
		xs = new XSensSensor(*mc);
	}
	catch (const std::exception &e) {
		std::cerr << e.what();
		std::exit(1);
	}
	cout << "done.\n";


  cout << "Press Ctl+c to stop program...\n";

// Don't need tuning thread in autorun version
//	std::thread shellThread(__gain_tuning_shell, mc, alerts);
	

  while(!terminate_flag) {
    getEstopStatus();

    if (estopPressed)
      terminate_flag = true;

  }

//	shellThread.join();

  mc->stop();

  delete xs;
  delete mc;
  delete alerts;

  return 0;



/* OLD CODE HERE */


/*
  int done = 0;
  while (!done) {
    getBrakeStatus();				// Comment out for no motor control
    getMotorFaultStatuses();		// Comment out for no motor control
    printIP();
    printSeparator();
    printMotorFaults();				// Comment out for no motor control
    //printVNStatus();
    printBrakeStatus();				// Comment out for no motor control
    //printSharedMemStatus();
    printSeparator();
    //printXSData();
    printSeparator();
    printEstopStatus();
    printMenu();

    int choice = 0;
    while (!(cin>>choice)) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Please make a valid selection: ";
    }

    switch (choice) {
      case 1:
        setBrake(!brakeStatus);
        //cout << "Not yet implemented.\n";
        break;
      case 2:
        //cout << "Not yet implemented.\n";
        clearMotorFaults();
        break;
      case 3:
        if (!brakeStatus)
          manualMove(1);
        else
          cout << "** Brake is ON! **\n";
        break;
      case 4:
        if (!brakeStatus)
          manualMove(2);
        else
          cout << "** Brake is ON! **\n";
        break;
      case 5:
        if (!brakeStatus)
          done = 1;
        else
          cout << "** Brake is ON! **\n";
        break;
      case 6:
        ServiceMode();
        break;
      case 7:
        std::exit(0);
        break;
    }
  }

  MotorController *mc;
  try {
    mc = new MotorController(xssm, alerts);
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  };

  cout << "Press Ctl+c to stop program...\n";

	std::thread shellThread(__gain_tuning_shell, mc, xssm, alerts);

  while(!terminate_flag) {
    getEstopStatus();

    if (estopPressed)
      terminate_flag = true;

    std::this_thread::yield();
  }

	shellThread.join();

  //vn->stop();
  mc->stop();

  delete mc;
  delete xs;
  delete alerts;

*/
}




/**** Function Implementations ****/

char* getIPAddress(const char* adapter) {
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family = AF_INET;
  memcpy(ifr.ifr_name, adapter, IFNAMSIZ-1);
  ioctl(fd, SIOCGIFADDR, &ifr);
  close(fd);
  strcpy(ipAddress, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
  return ipAddress;
}

void printSeparator() {
  cout << "------------------------------------\n";
}

bool getEstopStatus() {
	// Temporary --> seeing if this works to avoid the Estop
	return false;
	// End Temporary

  estopPressed = !digitalRead(ESTOP_PIN);
  return estopPressed;
}

void printEstopStatus() {
  getEstopStatus();

  if (estopPressed) {
    printSeparator();
    cout << "** ESTOP PRESSED! **\n";
    printSeparator();
  }

}
void printIP() {
  cout << "Wired: " << getIPAddress("eth0") << "\n";
  cout << "Wireless: " << getIPAddress("wlan0") << "\n";
}

void printMotorFaults() {
  if (motorFaultStatus) {
    if (motorFaultStatus1)
      cout << "Motor 1 is faulted\n";
    if (motorFaultStatus2)
      cout << "Motor 2 is faulted\n";
  }
  else {
    cout << "No Motor Faults\n";
  }
}

void printXSStatus() {
	cout << "XSens Sensor Status...\n";
}

bool getBrakeStatusMotor(uint16_t ampID) {

	if (ampID == 1) {
		MotorController mc(1);
		return mc.getBrakeStatus();
	}
	else {
		MotorController mc(2);
		return mc.getBrakeStatus();
	}
	
	/*
  bool status;
  MotorController* mc;

  if (ampID == 1)
    mc = new MotorController(1);
  else
    mc = new MotorController(2);

  status = mc->getBrakeStatus();
  delete mc;
  return status;
  */
}

void getBrakeStatus() {
  brakeStatus1 = getBrakeStatusMotor(1);
  brakeStatus2 = getBrakeStatusMotor(2);

  brakeStatus = brakeStatus1 || brakeStatus2;
}

void setBrake(bool brakeOn) {
  MotorController* mc1 = new MotorController(1);
  MotorController* mc2 = new MotorController(2);
  mc1->setBrake(brakeOn);
  mc2->setBrake(brakeOn);
  delete mc1;
  delete mc2;
}

void clearMotorFaults() {
  MotorController* mc1 = new MotorController(1);
  MotorController* mc2 = new MotorController(2);
  mc1->clearFaults();
  mc2->clearFaults();
  delete mc1;
  delete mc2;
}

bool getMotorFaultStatuses() {
  MotorController mc1(1);
  MotorController mc2(2);

// TODO: FIX THIS!
  motorFaultStatus1 = true; //mc1.getFaultStatus(1);
  motorFaultStatus2 = true; // mc2.getFaultStatus();
  motorFaultStatus = motorFaultStatus1 || motorFaultStatus2;
  return motorFaultStatus;
}

void printBrakeStatus() {
  cout << "Brake Status -  ";

  if (brakeStatus1)
    cout << "Motor 1:ON ";
  else
    cout << "Motor 1:OFF ";

  if (brakeStatus2)
    cout << "Motor 2:ON\n";
  else
    cout << "Motor 2:OFF\n";
}

void printSharedMemStatus() {
  cout << "Shared Memory Status...\n";
}

void printXSData() {
	// Test to make sure the sensor object exists
	if (!xs) {
		cout << "Error: no XSens sensor detected.\n";
		return;
	}

	float pitch = xs->getPitch();
	float roll  = xs->getRoll();

	cout << "Pitch = " << pitch << "\n";
	cout << "Roll  = " << roll << "\n";
}

void printMenu() {
  cout << "1) Turn Brake ";
  cout << ((brakeStatus) ? "OFF\n" : "ON\n");

  cout << "2) Clear Faults\n";

  cout << ((brakeStatus) ? " " : "3");
  cout << ") Manual Control - Motor 1\n";

  cout << ((brakeStatus) ? " " : "4");
  cout << ") Manual Control - Motor 2\n";

  cout << ((brakeStatus) ? " " : "5");
  cout << ") Run Mode\n";

  cout << "6) Service Mode\n";
  cout << "7) Shut Down\n";
}

void ServiceMode() {
	cout << "Press  'Enter' to show Roll and Pitch Values. Press 'q' to quit.\n";
	int done = 0;
	while (!done) {
	  	char choice;
	  	choice = getchar();
	  	if (choice == 'q') {
		 	done = 1;
		 	break;
		}
	  	else {
			cout << "Press  'Enter' to show Roll and Pitch Values. Press 'q' to quit.\n";
			printXSData();
	  	}
	}
	return;
}

void manualMove(uint16_t ampID) {
  //MotorController* mc;

  MotorController mc(ampID);

  cout << "Use 'l' and 'r' to move the motor. Press 'q' to quit.\n";

  int done = 0;
  while (!done) {
    char choice;
    //while (!(cin>>choice)) {
    //  cin.clear();
    //}
    //printVNData();
    choice = getchar();

	 int val;

    switch (choice) {
      case 'l':
        //cout << "Left\n";
		  cin >> val;
        mc.move(val, false);
        usleep(500000);
		  printXSData();
        break;
      case 'r':
        //cout << "Right\n";
		  cin >>val;
        mc.move(val, true);
        usleep(500000);
		  printXSData();
        break;
      case 'q':
        //delete mc;
        done = 1;
        return;
        break;
    }
  }
}

void printAmpParameters(MotorController *mc) {

	int8 mode = 0;
	mc->getModeOfOperation(1, mode);
	std::cout << "Pitch Amplifier Mode of Operation: " << ((mode == 1) ? "Profile Position" : (mode == 3) ? "Profile Velocity" : (mode == 4) ? "Profile Torque" : "Other") << std::endl;
	mc->getModeOfOperation(2, mode);
	std::cout << "Roll  Amplifier Mode of Operation: " << ((mode == 1) ? "Profile Position" : (mode == 3) ? "Profile Velocity" : (mode == 4) ? "Profile Torque" : "Other") << std::endl;

	std::cout << std::endl;

	std::cout << "Pitch AmpMode: ";
	mc->getAmpMode(1);
	std::cout << "Roll  AmpMode: ";
	mc->getAmpMode(2);

	std::cout << std::endl;

	uint32 data = 0;
	mc->getProfileAcceleration(1, data);
	std::cout << "Pitch Profile Acceleration (10 counts/s/s): " << data << std::endl;
	mc->getProfileDeceleration(1, data);
	std::cout << "Pitch Profile Deceleration (10 counts/s/s): " << data << std::endl;
	mc->getProfileAcceleration(2, data);
	std::cout << "Roll  Profile Acceleration (10 counts/s/s): " << data << std::endl;
	mc->getProfileDeceleration(2, data);
	std::cout << "Roll  Profile Deceleration (10 counts/s/s): " << data << std::endl;
	std::cout << std::endl;

	mc->getVLoopMaxVelocity(1, data);
	std::cout << "Pitch V Loop Max Velocity (.1 counts/s):   " << data << std::endl;
	mc->getVLoopMaxVelocity(2, data);
	std::cout << "Roll  V Loop Max Velocity (.1 counts/s):   " << data << std::endl;

	int32 datav;
	mc->getTargetVelocity(1, datav);
	std::cout << "Pitch Target Velocity (.1 counts/s):       " << datav << std::endl;
	mc->getTargetVelocity(2, datav);
	std::cout << "Roll  Target Velocity (.1 counts/s):       " << datav << std::endl;

	int32 data2 = 0;
	mc->getTargetPosition(1, data2);
	std::cout << "Pitch Target Position (counts):            " << data2 << std::endl;
	mc->getTargetPosition(2, data2);
	std::cout << "Roll  Target Position (counts):            " << data2 << std::endl;

	int16 data3 = 0;
	mc->getPp(1, data3);
	std::cout << "Pitch Pp:            " << data3 << std::endl;
	mc->getPp(2, data3);
	std::cout << "Roll Pp:             " << data3 << std::endl;

	int16 data4 = 0;
	mc->getVp(1, data4);
	std::cout << "Pitch Vp:            " << data4 << std::endl;
	mc->getVp(2, data4);
	std::cout << "Roll Vp:             " << data4 << std::endl;
}

/* Shell to be used in a thread to handle user commands while tuning gains
 *
 * Parameter: pointer to motor controller whose gains will be adjusted
 */
void __gain_tuning_shell(MotorController* mc, Alerts *alerts) {

	std::cout << "Gain Tuning Shell: Enter h for list of commands" << std::endl;
	std::cout << "> ";
	char input, input2;
	Gains gains = {0,0,0,0};
	bool done = false;
	int16 gainIn = 0;

	while (!done && std::cin >> input) {
		switch (input) {
			case 'h':
				std::cout << "Gain Tuning Shell Comands:\np: update pitch gains\nr: update roll gains\ns: status (current gains)\nh: help\nq: quit" << std::endl;
				break;
			case 'q':
				done = true;
				break;
			case 'p':
				std::cout << "Enter new gains for Pitch axis (K Kp Ki Kd): ";
				std::cin >> gains.K >> gains.Kp >> gains.Ki >> gains.Kd;
				mc->setPIDGains(1, &gains);
				mc->getPIDGains(1, &gains);
				std::cout << "Gains set to: " << gains.K << " " << gains.Kp << " " << gains.Ki << " " << gains.Kd << std::endl;
				break;
			case 'r':
				std::cout << "Enter new gains for Roll axis (K Kp Ki Kd): ";
				std::cin >> gains.K >> gains.Kp >> gains.Ki >> gains.Kd;
				mc->setPIDGains(2, &gains);
				mc->getPIDGains(2, &gains);
				std::cout << "Gains set to: " << gains.K << " " << gains.Kp << " " << gains.Ki << " " << gains.Kd << std::endl;
				break;
			case 's':
				std::cout << std::endl;
				std::cout << "Pitch Gains: " << std::endl;
				mc->getPIDGains(1, &gains);
				std::cout << "K: " << gains.K << " | Kp: " << gains.Kp << " | Ki: " << gains.Ki << " | Kd: " << gains.Kd << std::endl;
				std::cout << "Roll Gains: " << std::endl;
				mc->getPIDGains(2, &gains);
				std::cout << "K: " << gains.K << " | Kp: " << gains.Kp << " | Ki: " << gains.Ki << " | Kd: " << gains.Kd << std::endl;
				break;
			case 'd':
				std::cout << std::endl <<  "Which live output to toggle? (p/r): " <<std::endl;
				std::cin >> input2;
				if(input2 == 'p') {
					std::cout << "Toggling pitch output" <<std::endl;
					mc->togglePrintLiveInfo(1);
				}
				else if (input2 == 'r') {
					std::cout << "Toggling roll output" <<std::endl;
					mc->togglePrintLiveInfo(2);
				}
				else
					std::cout<<"Invalid Input"<<std::endl;
				break;
			case 'c':
				std::cout << std::endl << "Cycling motor controller" <<std::endl;
				mc->stop();
				delete mc;
				  try {
					 mc = new MotorController(alerts);
				  } catch (const std::exception &e) {
					 std::cerr << e.what();
					 std::exit(1);
				  };
				break;
			case 'f':
				std::cout << "\nFaults: Pitch: " << mc->getFaultStatus(1) << " Roll: " << mc->getFaultStatus(2) << std::endl;
				break;
			case 'a':
				printAmpParameters(mc);
				break;
			case 'g':
				std::cin >> input2 >> gainIn;
				if(input2 == 'p') {
					std::cout << "Setting Pitch Pp" <<std::endl;
					mc->setPp(1, gainIn);
				}
				else if (input2 == 'r') {
					std::cout << "Setting Roll Pp" <<std::endl;
					mc->setPp(2, gainIn);
				}
				break;
			default:
				std::cout << "Invalid Input" << std::endl;
		}
		std::cout << "> ";
	}
	std::cout << "Exiting Gain Tuning Shell" << std::endl;
}

