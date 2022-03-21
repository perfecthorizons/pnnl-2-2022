#ifndef _ALERTS_H
#define _ALERTS_H

#include <cstdio>
#include <ctime>
#include <string>
#include <cstring>

#include "debug.h"

#if __has_include(<mqueue.h>)
  #include <mqueue.h>
#else
  #warning mqueue.h not detected. Compiling without alert server. Alerts will be logged instead.
#endif

constexpr static const uint16_t MSG_MAX = 512;
constexpr static const uint16_t MSG_SIZE = 1024;
constexpr static const uint16_t MSG_MAX_WRITERS = 15;
constexpr static const uint16_t MSG_PMODE = 0655;

class Alerts {
public:
  Alerts();
  ~Alerts();

  template<typename ...Args>
  bool fatal_error(const char *, Args &...);

  template<typename ...Args>
  bool nonfatal_error(const char *, Args &...);

  template<typename ...Args>
  bool warning(const char *, Args &...);

  template<typename ...Args>
  bool diagnostic(const char *, Args &...);

private:
  #if __has_include(<mqueue.h>)
    mqd_t mqfd;
    struct mq_attr attr;
  #endif

  bool send(const char *);

  template<typename ...Args>
  bool prepare_send(const char *, const char *, Args &...);
};

template<typename ...Args>
bool Alerts::fatal_error(const char *message, Args &...args) {
  return prepare_send(message, "FATAL ERROR", args...);
}

template<typename ...Args>
bool Alerts::nonfatal_error(const char *message, Args &...args) {
  return prepare_send(message, "NON-FATAL ERROR", args...);
}

template<typename ...Args>
bool Alerts::warning(const char *message, Args &...args) {
  return prepare_send(message, "WARNING", args...);
}

template<typename ...Args>
bool Alerts::diagnostic(const char *message, Args &...args) {
  return prepare_send(message, "DIAGNOSTIC", args...);
}

template<typename ...Args>
bool Alerts::prepare_send(const char *message, const char *prepend, Args &...args) {
  char format_buffer[MSG_SIZE] = "(%s) %s: ";
  std::memcpy(format_buffer+9, message, std::min<unsigned>(std::strlen(message), MSG_SIZE-9));

  char message_buffer[MSG_SIZE];

  time_t tm = std::time(NULL);
  struct tm* tm_info = localtime(&tm);

  char timestamp[26];
  strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  int count = snprintf(message_buffer, MSG_SIZE, format_buffer, timestamp, prepend, args...);
  if (count < 0 || count > MSG_SIZE) {
    LOG(2, "Message too large (truncated).");
  }

  return send(message_buffer);
}

#endif
