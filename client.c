#include <netdb.h>

#include <stdio.h>

#include <time.h>

#include <stdio.h>

#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>

#include <sys/socket.h>

#define MAX 80
#define PORT 2503
#define SA struct sockaddr

int lets = 0;

void * LifeVerifier(void * sockfd) {

  int * port = (int * ) sockfd;
  int connfd = * port;

  while (1) {
    if (lets == 0) {
      write(connfd, "lifeTime", sizeof("lifeTime"));
      sleep(1);
    } else {
      break;
    }

  }
}
void * writeThread(void * sockfd) {

  int * port = (int * ) sockfd;
  int connfd = * port;

  while (1) {
    int index = 0;
    char command[2000] = "";
    printf("\n");
    while ((command[index++] = getchar()) != '\n');
    command[strcspn(command, "\n")] = 0;
    write(connfd, command, sizeof(command));

    if (strcmp(command, "QUIT") == 0) {
      lets = 1;
      break;
    }

  }
}
void * readThread(void * sockfd) {

  int * port = (int * ) sockfd;
  int connfd = * port;
  while (1) {
    char respond[2500] = "";
    read(connfd, respond, sizeof(respond));
    printf("\n => Server : %s\n", respond);

    if (lets > 0) {
      break;
    }

  }
}

int main(int argc, char * argv[]) {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero( & servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]);
  servaddr.sin_port = htons(atoi(argv[2]));

  if (connect(sockfd, (SA * ) & servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else
    printf("connected to the server..\n");
  //MyThread(sockfd);

  char buff[MAX];
  int studentID;
  int n;
  read(sockfd, buff, sizeof(buff));

  if (strcmp("Clients Limit Reached", buff) != 0) {
    printf("\nConnection Established\n(%s)\n");
    //Getting Student ID
    printf("ID>");
    scanf("%d", &studentID);
    
    printf("\nFOLLOWING COMMANDS CAN BE USED IN PROGRAM \n => JOIN <nickname> <relaname>\n => WHOIS <target> \n => MSG <target> <text>\n => TIME\n => ALIVE\n => QUIT\n");

    pthread_t thread_id;
    pthread_create( & thread_id, NULL, writeThread, & sockfd);

    pthread_t thread_id2;
    pthread_create( & thread_id2, NULL, readThread, & sockfd);

    pthread_t thread_id3;
    pthread_create( & thread_id3, NULL, LifeVerifier, & sockfd);

    pthread_join(thread_id);
    pthread_join(thread_id2);
    pthread_join(thread_id3);
		
	if (lets>0)
	{
		close(sockfd);
	}

  } else {
    printf("\n => Server : (%s)\n");
  }

}
