#include <thread>
#include <random>
#include <chrono>

#include "data_logger.h"
#include "shared_memory.h"
#include "literals.h"

constexpr static const char *data_path = "testdata.csv";

void random_data_writer(shared_memory<uint64_t, uint64_t> *);

int main(int argc, char *argv[]) {
  shared_memory<uint64_t, uint64_t> m;
  std::thread t(random_data_writer, &m);
  data_logger<uint64_t, uint64_t> logger(&m, data_path, 2_hz);
  t.join();
}

void random_data_writer(shared_memory<uint64_t, uint64_t> *m) {
  while (true) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    m->write(dis(gen), dis(gen));
    std::this_thread::sleep_for(3_hz);
  }
}
