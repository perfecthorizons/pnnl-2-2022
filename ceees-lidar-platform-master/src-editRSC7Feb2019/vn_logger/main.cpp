#include "shared_memory.h"
#include "vectornav.h"
#include "data_logger.h"
#include "literals.h"
#include "debug.h"

#include <thread>

constexpr static const char *vn_data_path="/home/pi/data_static/";

static volatile bool terminate_flag = false;

void interrupt_handler(int s) {
terminate_flag = true;
}

int main(int argc, char *argv[]) {

  signal(SIGINT, interrupt_handler);

  VectorNav *vn;
  try {
    vn = new VectorNav();
  } catch (const std::exception &e) {
    std::cerr << e.what();
    std::exit(1);
  }

  VNSM *vnsm = vn->get_shared_memory();

  data_logger<uint64_t, float, float, float, float, float, float, float, double, double, double, float, float, float, float, float, float, float, float, float> vn_logger(vnsm, vn_data_path, 20_hz);

  while(!terminate_flag) std::this_thread::yield();

  delete vn;
}
