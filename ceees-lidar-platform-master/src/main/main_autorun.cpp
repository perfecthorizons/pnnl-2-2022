/**********************************

main_autorun.cpp is a simplified version of main.cpp that is meant for automatic execution on system startup.
As such, it does not give the user a menu of choices like main.cpp; rather, it jumps directly into homing the 
system and running the stabilization code. To have it run automatically, create a cronjob that runs autorun.sh
at system reboot:


**********************************/
#include "dynamic_functions.h"
#include "motorcontroller.h"
#include "shared_memory.h"
//#include "vectornav.h"
#include "data_logger.h"
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

// XSens Sensor globals
constexpr static const char *xs_data_path="/home/pi/XSensData";
char XS_fn[50]; // Not sure what this is for
stringstream XS_filename;

static XSensSensor *xs = NULL;
Journaller *gJournal = 0; // This is needed for XSens sensor

// E-stop globals
static const int ESTOP_PIN = 1;
static volatile bool estopPressed = false;
static char ipAddress[15] = "0.0.0.0";


#ifdef PRODUCTION
  #include <unistd.h>
  #include <signal.h>
  #include <cerrno>
  #include "pid_file.h"

  static volatile bool terminate_flag = false;
  static bool brakeStatus1, brakeStatus2, brakeStatus;
  static bool motorFaultStatus1, motorFaultStatus2, motorFaultStatus;
//  static float pitch, roll;

  void interrupt_handler(int s) {
    terminate_flag = true;
  }

#endif

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

// TODO: FIX THIS
  motorFaultStatus1 = true; //mc1.getFaultStatus();
  motorFaultStatus2 = true; //mc2.getFaultStatus();
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

/* Shell to be used in a thread to handle user commands while tuning gains
 *
 * Parameter: pointer to motor controller whose gains will be adjusted
 */
void __gain_tuning_shell(MotorController* mc) {

	std::cout << "Gain Tuning Shell: Enter h for list of commands" << std::endl;
	std::cout << "> ";
	char input;
	Gains gains = {0,0,0,0};
	bool done = false;

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
				std::cout << "Gains set to: " << gains.K << " " << gains.Kp << " " << gains.Ki << " " << gains.Kd << std::endl;
				break;
			case 'r':
				std::cout << "Enter new gains for Roll axis (K Kp Ki Kd): ";
				std::cin >> gains.K >> gains.Kp >> gains.Ki >> gains.Kd;
				std::cout << "Gains set to: " << gains.K << " " << gains.Kp << " " << gains.Ki << " " << gains.Kd << std::endl;
				break;
			case 's':
				std::cout << "Pitch Gains: " << std::endl;
				mc->getPIDGains(1, &gains);
				std::cout << "K: " << gains.K << " | Kp: " << gains.Kp << " | Ki: " << gains.Ki << " | Kd: " << gains.Kd << std::endl;
				std::cout << "Roll Gains: " << std::endl;
				mc->getPIDGains(2, &gains);
				std::cout << "K: " << gains.K << " | Kp: " << gains.Kp << " | Ki: " << gains.Ki << " | Kd: " << gains.Kd << std::endl;
				break;
			default:
				std::cout << "Invalid Input" << std::endl;
		}
		std::cout << "> ";
	}
	std::cout << "Exiting Gain Tuning Shell" << std::endl;
}


int main(int argc, char *argv[]) {

  #ifdef PRODUCTION
    //if (daemon(1, 0) != 0) {
    //  LOG(0, "Error daemonizing process. Error Number " << errno << ".\n");
    //  std::exit(1);
    //}
    //PIDFile p("/tmp/casper2.pid");

    signal(SIGINT, interrupt_handler);

  #endif
  
  wiringPiSetup();
  pinMode(ESTOP_PIN, INPUT);
  pullUpDnControl(ESTOP_PIN, PUD_DOWN);

//  initscr();

//  printEstopStatus();

  /**************************** Uncomment for EBrake 
  getEstopStatus();
  if (estopPressed) {
    cout << "ESTOP is pressed! Please clear it before starting the application.\n";
    std::exit(1);
  }

  */

  Alerts *alerts;
  try {
    alerts = new Alerts();
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  }

  alerts->diagnostic("Stabilization Platform coming online.");


  /* XSens Integration begins here */
   cout << "Creating XSensSensor...";
  	try {
		xs = new XSensSensor();
	}
	catch (const std::exception &e) {
		std::cerr << e.what();
		std::exit(1);
	}
	cout << "done.\n";

	cout << "Creating XSSM...";
	XSSM *xssm = xs->get_shared_memory();
	cout << "done.\n";

   /* Uncomment for data logger --> the logger was not terminating correctly at program end so I commented it out
	cout << "Creating data_logger...";
	time_t ttt = time(nullptr);
	tm tmm = *localtime(&ttt);
	XS_filename << xs_data_path << put_time(&tmm, "%Y_%m_%d_%T.txt");
	XS_filename >> XS_fn;
	data_logger<uint64_t, float, float, float, float, float, float, float> xs_logger(xssm, xs_data_path, 20_hz);
	cout << "done.\n";
	*/

  /* --------------------- */
  	
	// Comment this out to get a full log of all Copley Motion Library function calls (motor controllers)
	//cml.SetDebugLevel(LOG_EVERYTHING);

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
    //printMenu();

	 done = 1;

	}
  

  MotorController *mc;
  try {
    mc = new MotorController(xssm, alerts);
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  };

  cout << "Press Ctl+c to stop program...\n";

	//std::thread shellThread(__gain_tuning_shell, mc);

  while(!terminate_flag) {
    getEstopStatus();

    if (estopPressed)
      terminate_flag = true;

    std::this_thread::yield();
  }

//	shellThread.join();

  //vn->stop();
  mc->stop();

  delete mc;
  delete xs;
  delete alerts;

}
