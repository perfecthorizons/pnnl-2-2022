#include "dynamic_functions.h"
#include "motorcontroller.h"
#include "shared_memory.h"
#include "vectornav.h"
#include "data_logger.h"
#include "literals.h"
#include "debug.h"
#include "alerts.h"
#include "stdio.h"
#include <thread>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
//#include <curses.h>


constexpr static const char *vn_data_path="/home/pi/data";
static const int ESTOP_PIN = 1;

static volatile bool estopPressed = false;
static VectorNav *vnav = NULL;
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

void printVNStatus() {
  cout << "VectorNav Status...\n";
}

bool getBrakeStatusMotor(uint16_t ampID) {
  bool status;
  MotorController* mc;

  if (ampID == 1)
    mc = new MotorController(1);
  else
    mc = new MotorController(2);

  status = mc->getBrakeStatus();
  delete mc;
  return status;
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
  MotorController* mc1 = new MotorController(1);
  MotorController* mc2 = new MotorController(2);

  motorFaultStatus1 = mc1->getFaultStatus();
  motorFaultStatus2 = mc2->getFaultStatus();
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
    cout << "Motor 2: OFF\n";
}

void printSharedMemStatus () {
  cout << "Shared Memory Status...\n";
}

void printVNData() {
  float pitch = vnav->getPitch();
  float roll = vnav->getRoll();

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
  else
   printVNData();
  }
return;
}
void manualMove(uint16_t ampID) {
  MotorController* mc;

  mc = new MotorController(ampID);

  cout << "Use 'l' and 'r' to move the motor. Press 'q' to quit.\n";

  int done = 0;
  while (!done) {
    char choice;
    //while (!(cin>>choice)) {
    //  cin.clear();
    //}
    //printVNData();
    choice = getchar();

    switch (choice) {
      case 'l':
        //cout << "Left\n";
        mc->move(false);
        usleep(500);
        printVNData();
        break;
      case 'r':
        //cout << "Right\n";
        mc->move(true);
        usleep(500);
        printVNData();
        break;
      case 'q':
        delete mc;
        done = 1;
        return;
        break;
    }
  }
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
  getEstopStatus();
  if (estopPressed) {
    cout << "ESTOP is pressed! Please clear it before starting the application.\n";
    std::exit(1);
  }

  Alerts *alerts;
  try {
    alerts = new Alerts();
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  }

  alerts->diagnostic("Stabilization Platform coming online.");


//  VectorNav *vn;
  try {
    vnav = new VectorNav();
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  }

  cout << "Creating VNSM...";
  VNSM *vnsm = vnav->get_shared_memory();
  cout << "done.\n";

  cout << "Creating data_logger...";
  data_logger<uint64_t, float, float, float, float, float, float, double, double, double, float, float, float, float, float, float, float, float, float, float, float> vn_logger(vnsm, vn_data_path, 20_hz);
  cout << "done.\n";

  int done = 0;
  while (!done) {
    getBrakeStatus();
    getMotorFaultStatuses();
    printIP();
    printSeparator();
    printMotorFaults();
    //printVNStatus();
    printBrakeStatus();
    //printSharedMemStatus();
    printSeparator();
    printVNData();
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
    mc = new MotorController(vnsm, alerts);
  } catch (const std::exception &e) {
    std:: cerr << e.what();
    std::exit(1);
  };

  cout << "Press Ctl+c to stop program...\n";

  while(!terminate_flag) {
    getEstopStatus();

    if (estopPressed)
      terminate_flag = true;

    std::this_thread::yield();
  }

  //vn->stop();
  mc->stop();

  //delete mc;
  delete vnav;
  delete alerts;

}
