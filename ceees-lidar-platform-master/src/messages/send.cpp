#include <iostream>
#include <string.h>
#include <mqueue.h>
#include <thread>
#include <chrono>

using namespace std;

#define PMODE 0655
#define MAX_WRITERS (5)

int main() {
  int status;
  struct mq_attr attr;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 256;

  char buffer[1000];
  int i = 0;
  while (1) {
    mqd_t mqfd = mq_open("/caspercppipc", O_WRONLY|O_CREAT|O_NONBLOCK, PMODE, &attr);
    if(mqfd == -1) {
       perror("Child mq_open failure");
       exit(0);
    }
    mq_getattr(mqfd, &attr);
    if (attr.mq_curmsgs < attr.mq_maxmsg - MAX_WRITERS) {
      sprintf(buffer, "WARNING: %d", i++);
      status = mq_send(mqfd, buffer, strlen(buffer)+1, 0);
      sprintf(buffer, "NON-FATAL ERROR: %d", i++);
      status = mq_send(mqfd, buffer, strlen(buffer)+1, 0);
      sprintf(buffer, "FATAL ERROR: %d", i++);
      status = mq_send(mqfd, buffer, strlen(buffer)+1, 0);
      sprintf(buffer, "DIAGNOSTIC: %d", i++);
      status = mq_send(mqfd, buffer, strlen(buffer)+1, 0);
      this_thread::sleep_for(chrono::milliseconds(1000));
      if (status == -1) {
        perror("mq_send failure\n");
      }
      else {
        printf("mq_send successful: %s\n", buffer);
      }
    }
    else {
      printf("mq is full, skipping message: %s\n", buffer);
    }
    mq_close(mqfd);
  }
  return 0;
}
