#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <cstdio>
#include <mqueue.h>
#include <fcntl.h>

using namespace std;

#define SERVER_PORT htons(50007)
#define PMODE 0655

int main() {
  char buffer[1000];
  int n;
  int serverSock = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = SERVER_PORT;
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

  // Wait for a client
  listen(serverSock, 1);

  while(true) {
    sockaddr_in clientAddr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &sin_size);

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 20;

    int pid = fork();
    if (pid < 0) {
      cerr << "ERROR on fork";
    }
    else if (!pid) {
      // In the child process.
      int count = 0;
      while (true) {
        memset(buffer, 0, 1000);

        mqd_t mqfd = mq_open("/caspercppipc", O_RDONLY|O_CREAT, PMODE, &attr);
        if(mqfd == -1) {
           perror("Child mq_open failure");
           exit(0);
        }

        int status = mq_receive(mqfd, buffer, 100, 0);
        if (status == -1) {
          perror("mq_receive failure\n");
        }
        else {
          //printf("mq_receive successful\n");
          //printf("Child received message: %s\n", buffer);
        }

        mq_close(mqfd);

        //sprintf(buffer, "test: %d ", count++);
        n = write(clientSock, buffer, strlen(buffer));
        cout << "Redirecting: " << buffer << endl;
        //cout << "Send Confirmation code  " << n << endl;
      }
      close(clientSock);
      return 0; // Exit the child program.
    }
    else {
      // In the parent process.
      close(clientSock);
      // The point of this parent process is to re-open the child process,
      // which is killed every time the connection is cut.
      waitpid(pid, NULL, 0);
    }
  }
  return 0;
}
