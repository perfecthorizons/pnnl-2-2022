#ifndef _DATA_LOGGER_H
#define _DATA_LOGGER_H

#define PATH_SEPERATOR "/"

#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <thread>

#include "shared_memory.h"

template<typename ...Types>
class data_logger {
public:
  data_logger(shared_memory<Types...> *what, std::string where, std::chrono::nanoseconds how_often, uint32_t max_record_count = 10000, uint32_t flush_every_n_records = 10)
    : storage_path(where),
      fs(where, std::fstream::out),
      logger_continue(true),
      data(what),
      period(how_often),
      max_record_count(max_record_count),
      flush_every_n_records(flush_every_n_records)
  {
    logger = std::thread(&data_logger::__data_logger, this);
  }

  ~data_logger() {
    LOG(2, "Stopping data logger...");
    logger_continue = false;
    logger.join();

    LOG(2, "Closing data output file.");
    fs.close();
  }

  void open_file_handle() {

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream filename;
    filename << std::put_time(&tm, "%c %z");
    filename << ".csv";

    std::string filepath = storage_path + PATH_SEPERATOR + filename.str();
    fs = std::fstream(filepath, std::fstream::out);

    if (!fs.is_open()) {
      LOG(1, "Unable to open data output file: " << filepath << "\n");
      throw "Unable to open file!";
    }

    if (data->is_named()) {
      LOG(3, "Writing headers to data file");
      fs << data->names(", ");
    } else {
      LOG(3, "Shared memory is not named; cannot write headers to data file.");
    }

    records_written = 0;
  }

  void __data_logger() {
    while (logger_continue) {
      std::this_thread::sleep_for(period);

      /*************************************************************************
       * Note: writing a newline at the end of each record may implicitly flush
       * the output buffer, however, not necessarily; thus we will explicitly
       * flush the output buffer every n lines.
       ************************************************************************/

      if (!fs.is_open()) {
        LOG(1, "WARNING: LOGGING FILE IS NOT OPEN!");
        continue;
      }

      std::time_t t = std::time(nullptr);
      std::tm tm = *std::localtime(&t);
      std::stringstream filename;
      fs << std::put_time(&tm, "%c %z") << ", ";

      fs << data->read(", ") << std::endl;

      if (records_written % flush_every_n_records == flush_every_n_records - 1) {
        fs << std::flush;
      }
    }
  }

private:
  std::string storage_path;
  std::fstream fs;
  bool logger_continue;
  std::thread logger;
  shared_memory<Types...> *data;
  std::chrono::nanoseconds period;
  uint32_t records_written;
  uint32_t max_record_count;
  uint32_t flush_every_n_records;
};

#endif
