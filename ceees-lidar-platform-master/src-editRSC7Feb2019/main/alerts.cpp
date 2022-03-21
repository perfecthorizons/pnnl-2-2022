#include "alerts.h"
#if __has_include(<mqueue.h>)
  #include <cerrno>
  #include <chrono>
  #include <thread>
  #include <cstring>
  #include <string>
  #include <mqueue.h>

  #include "debug.h"


  Alerts::Alerts() {
    attr.mq_maxmsg = MSG_MAX;
    attr.mq_msgsize = MSG_SIZE;
    mqfd = mq_open("/caspercppipc", O_WRONLY|O_CREAT|O_NONBLOCK, MSG_PMODE, &attr);

    if(mqfd == -1) {
       LOG(1, "Child mq_open failure");
       LOG(1, "Error:" << strerror(errno));
       throw "Child mq_open failure";
    }
  }

  Alerts::~Alerts() {
    mq_close(mqfd);
  }

  bool Alerts::send(const char *message) {
    LOG(2, message);
/*    mq_getattr(mqfd, &attr);
    if (attr.mq_curmsgs >= attr.mq_maxmsg - MSG_MAX_WRITERS) {
      LOG(2, "Too many messages in the queue to send.");
      LOG(2, "Dropping message: " << message);
      return false;
    }

    if (mq_send(mqfd, message, strlen(message)+1, 0) != 0) {
      LOG(2, "Error sending message: " << strerror(errno));
      LOG(2, "Dropping message: " << message);
      return false;
    }
*/
    return true;
  }
#else
  Alerts::Alerts() {;}
  Alerts::~Alerts() {;}
  bool Alerts::send(const char *message) {
    LOG(0, message);
  }
#endif
