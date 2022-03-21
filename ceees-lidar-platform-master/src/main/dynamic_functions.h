#ifndef _DYNAMIC_FUNCTIONS_H
#define _DYNAMIC_FUNCTIONS_H

#include <map>
#include <string>
#include <shared_mutex>
#include <initializer_list>
#include <memory>
#include <thread>

#include "debug.h"

// TODO: convert std::vector<std::string> symbols to std::initializer_list<std::string> symbols or a parameter pack w/ fold expressions!

struct DynamicFunction {
  void *fn_handle;
  std::shared_mutex *fn_guard;
  DynamicFunction(void *fn_handle) : fn_handle(fn_handle) {
    fn_guard = new std::shared_mutex ();
  };
  ~DynamicFunction() {
    delete fn_guard;
  }
};

class DynamicFunctions {
  public:
    DynamicFunctions(std::string object_file, std::initializer_list<std::string> symbols);
    ~DynamicFunctions();

    template <typename ReturnType, typename ...ArgTypes>
    ReturnType call(std::string, ArgTypes...);

  private:
    void FileMonitor(std::string object_file, std::initializer_list<std::string> symbols);
    void UpdateHandles(std::string object_file, std::initializer_list<std::string> symbols);
    std::string object_file_hash;
    void *handle;
    std::string HashFile(std::string filename);

    std::map<std::string, DynamicFunction *> fn_handles;
    std::shared_mutex fn_handles_guard;
    bool file_monitor_continue;
    std::thread file_monitor;
};

/*********************************************************************** Call */
template <typename ReturnType, typename ...ArgTypes>
ReturnType DynamicFunctions::call(std::string symbol, ArgTypes... args) {

  LOG(3, "Attemping to acquire fn_handles_guard lock.");
  std::shared_lock<std::shared_mutex> guard(fn_handles_guard);

  DynamicFunction *dynamic_function = fn_handles[symbol];

  LOG(3, "Attemping to acquire fn_guard lock for function: " << symbol);
  std::shared_lock<std::shared_mutex> fn_guard(*(dynamic_function->fn_guard));

  LOG(2, "Calling dynamic function: " << symbol);

  ReturnType (*fn)(ArgTypes...);
  *(void **) (&fn) = dynamic_function->fn_handle;

  return (*fn)(args...);
}

#endif
