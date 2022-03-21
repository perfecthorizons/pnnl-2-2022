#include <thread>
#include <shared_mutex>
#include <chrono>
#include <tuple>
#include <fstream>
#include <initializer_list>
#include <dlfcn.h>

#include "dynamic_functions.h"
#include "md5.h"

#include "debug.h"

#ifndef MANUAL_TRANSFORM_MONITOR_PERIOD
  #define MANUAL_TRANSFORM_MONITOR_PERIOD (1)
#endif

/**************************************************************** Constructor */
DynamicFunctions::DynamicFunctions(std::string object_file, std::initializer_list<std::string> symbols) {
  UpdateHandles(object_file, symbols);
  file_monitor_continue = true;
  file_monitor = std::thread(&DynamicFunctions::FileMonitor, this, object_file, symbols);
}

/***************************************************************** UpdateHandles
 *
 * This function opens the file path specified by object_file (intended to be
 * a shared object file (*.so)) and loads the symbols specified by the strings
 * in the vector symbols.
 *
 * ****************************************************************************/
void DynamicFunctions::UpdateHandles(std::string object_file, std::initializer_list<std::string> symbols) {
  object_file_hash = HashFile(object_file);

  handle = dlopen(object_file.c_str(), RTLD_LAZY);

  // If handle is null, could not open file
  if (!handle) {
    LOG(1, "Could not open file.");
    throw "Could not open file.";
  }

  for (const std::string &symbol : symbols) {
    // Clear existing errors
    dlerror();

    LOG(3, "Looking up symbol: " << symbol);

    // Look up the symbol
    void *fn = dlsym(handle, symbol.c_str());

    // Check for errors
    char *error = dlerror();
    if (NULL != error) {
      LOG(1, error << "\n");
      throw error;
    }

    // Lock the handle table and update it with the new handle
    {
      std::unique_lock<std::shared_mutex> guard(fn_handles_guard);
      if (fn_handles.count(symbol)) delete fn_handles[symbol];
      fn_handles[symbol] = new DynamicFunction(fn);
    }
  }

  LOG(2, "Finished updating handles")
}

/***************************************************************** Destructor */
DynamicFunctions::~DynamicFunctions() {
  file_monitor_continue = false;
  file_monitor.join();
  dlclose(handle);
}

/******************************************************************* HashFile */
std::string DynamicFunctions::HashFile(std::string filename) {
  std::ifstream file(filename, std::ifstream::binary);
  MD5_CTX md5_context;
  MD5_Init(&md5_context);
  char buffer[16384];
  while (file.good()) {
    file.read(buffer, sizeof(buffer));
    MD5_Update(&md5_context, buffer, file.gcount());
  }
  file.close();
  unsigned char hash[MD5_DIGEST_LENGTH];
  MD5_Final(hash, &md5_context);
  return std::string(reinterpret_cast<char *>(hash));
}

/**************************************************************** FileMonitor */
void DynamicFunctions::FileMonitor(std::string object_file, std::initializer_list<std::string> symbols) {
  while (file_monitor_continue) {
    std::string current_hash = HashFile(object_file);

    LOG(3, "Checking hash of dynamic function file:" << current_hash);

    if (object_file_hash != current_hash) {

      LOG(1, "Detected different hashes. Reloading symbols...");

      dlclose(handle);
      UpdateHandles(object_file, symbols);
    }
    std::this_thread::sleep_for(std::chrono::seconds(MANUAL_TRANSFORM_MONITOR_PERIOD));
  }
}
