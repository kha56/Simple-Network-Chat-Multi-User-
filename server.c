#include <stdio.h>

#include <netdb.h>

#include <time.h>

#include <netinet/in.h>

#include <stdlib.h>

#include <string.h>

#include <sys/socket.h>

#include <sys/types.h>

#define MAX 80
#define PORT 2504
#define SA struct sockaddr

int timeout = 0;
int aliveStatus[150][250];
int userPort[150];
char userNick[150][200];
char userName[151][202];
int userCount = 0;
int messageNumber = 0;

void * MyThread(void * sockfd) {

  char myUser[200] = "-";
  int * port = (int * ) sockfd;
  int connfd = * port;
  int myIndex = -1;
  int lifeTime = 1;

  char buff[MAX];
  char command[MAX];
  int n;
  char EXIT[200] = "";

  write(connfd, "Welcome to SNC", sizeof("Welcome to SNC"));

  while (1) {

    char Command[200] = "";
    read(connfd, Command, sizeof(Command));

    char * token = strtok(Command, " ");

    //JOIN command
    while (token != NULL) {
      if (strcmp(token, "JOIN") == 0) {

        printf("\n-----Command Recieved = (%s)", Command);
        token = strtok(NULL, " ");

        //Max 20 characters for Nickname
        char nickName[20] = "";

        strcpy(nickName, token);
        strcpy(myUser, token);

        //Max 20 Characters for realName
        char fullName[20] = "";

        token = strtok(NULL, " ");

        while (token != NULL) {
          strcat(fullName, token);
          strcat(fullName, " ");
          token = strtok(NULL, " ");
        }

        printf("\nHERE USER COUNT INDEX (%d)", userCount);

        int check = 0;
        for (int i = 0; i < userCount; i++) {

          if (strcmp(userNick[i], nickName) == 0) {
            write(connfd, "please repeat command with new nickname", sizeof("please repeat command with new nickname"));
            check = 1;
            break;
          } else if (strcmp(userName[i], fullName) == 0) {
            write(connfd, "Client with same realname already joined", sizeof("Client with same realname already joined"));
            check = 1;
            break;
          }

        }

        if (check == 0) {

          for (int i = 0; i < userCount; i++) {

            if (strcmp(aliveStatus[i], "ALIVE") == 0) {
              char reply[2000] = "A New User ";
              strcat(reply, nickName);
              strcat(reply, " has joined");
              write(userPort[i], reply, sizeof(reply));
            }
          }
          //Copying nickname and fullname to the Array
          lifeTime = 1;
          myIndex = userCount;
          strcpy(aliveStatus[userCount], "ALIVE");
          strcpy(userNick[userCount], nickName);
          strcpy(userName[userCount], fullName);
          userPort[userCount] = connfd;
          userCount = userCount + 1;

          char reply[2000] = "";
          strcpy(reply, nickName);
          strcat(reply, ", Welcome to SNC !");
          write(connfd, reply, sizeof(reply));

        }


      //MSG command
      } else if (strcmp(token, "MSG") == 0) {

        printf("\nCommand Recieved = (%s)", Command);
        token = strtok(NULL, " ");

		if (token==NULL)
		{
	   write(connfd, "NickName <target> is missing", sizeof("NickName <target> is missing"));		
	   break;
		}
		
        char nickName[500] = "";

        strcpy(nickName, token);

        printf("\nnickName (%s)", nickName);

        //Max length of Msg
        char message[256] = "";

        token = strtok(NULL, " ");
		if (token==NULL)
		{
	   write(connfd, "message to <target> is missing", sizeof("message to <target> is missing"));		
	   break;
		}


        while (token != NULL) {
          strcat(message, token);
          strcat(message, " ");
          token = strtok(NULL, " ");
        }

        int check = 0;
        for (int i = 0; i < userCount; i++) {

          if (strcmp(userNick[i], nickName) == 0 && strcmp(aliveStatus[i], "ALIVE") == 0) {
            char reply[2000] = "";
            strcat(reply, myUser);
            strcat(reply, " : ");
            strcat(reply, message);
            write(userPort[i], reply, sizeof(reply));
            check = 1;
          }
        }
        if (check == 0) {
          write(connfd, "No username found with this nick", sizeof("No username found with this nick"));

        }

      //Whois Command
      } else if (strcmp(token, "WHOIS") == 0) {
        printf("\nCommand Recieved = (%s)", Command);
        token = strtok(NULL, " ");

        char nick[100] = "";
        strcpy(nick, token);
        int check = 0;
        for (int i = 0; i < userCount; i++) {

          if (strcmp(userNick[i], nick) == 0) {
            check = 1;
            strcat(nick, " ");
            strcat(nick, userName[i]);
            write(connfd, nick, sizeof(nick));

          }

        }

        if (check == 0) {
          write(connfd, "error - no user nick found", sizeof("error - no user nick found"));
        }

      //Time Command
      } else if (strcmp(token, "TIME") == 0) {

        printf("\nCommand Recieved = (%s)", Command);
        time_t t; // not a primitive datatype
        time( & t);
        char reply[250] = "";
        strcpy(reply, ctime( & t));
        printf("Servers Local Time is = (%s)", reply);
        write(connfd, reply, sizeof(reply));
      } 
      //Quit Command
      else if (strcmp(token, "QUIT") == 0) {
        printf("\nCommand Recieved = (%s)", Command);

        char reply[100] = "Bye ";
        strcat(reply, myUser);
        write(connfd, reply, sizeof(reply));

        char reply2[100] = "";
        strcat(reply2, myUser);
        strcat(reply2, " has stopped chatting");

        for (int i = 0; i < userCount; i++) {

          if ((strcmp(myUser, userNick[i]) != 0) && (strcmp(aliveStatus[i],"ALIVE") == 0)) {
            write(userPort[i], reply2, sizeof(reply2));

          } else {
            write(userPort[i], "BYEEE", sizeof("BYEEE"));
            strcpy(aliveStatus[i], "DEAD");
          }

        }
        strcpy(EXIT, "BYEEE");
        close(connfd);

    //ALIVE command
      } else if (strcmp(token, "ALIVE") == 0) {
        lifeTime = 0;
        for (int i = 0; i < userCount; i++) {

          if ((strcmp(myUser, userNick[i]) == 0)) {
            write(userPort[i], "OK", sizeof("OK"));
            strcpy(aliveStatus[i], "ALIVE");
          }

        }
      } else if (strcmp(token, "lifeTime") == 0) {
        if (myIndex>=0)
        {
        lifeTime = lifeTime + 1;
        if (lifeTime >= timeout) {
         
 			write(connfd, "You Are Timed Out", sizeof("You Are Timed Out"));
              strcpy(aliveStatus[myIndex], "DEAD");

          while (1) {

            char aliveRequest[200] = "";
            read(connfd, aliveRequest, sizeof(aliveRequest));

            if (strcmp(aliveRequest, "ALIVE") == 0) {
              lifeTime = 0;
                  write(userPort[myIndex], "OK", sizeof("OK"));
                  strcpy(aliveStatus[myIndex], "ALIVE");
                  break;
          }
          
          

        }
        }
      }
      } 
      //Input Valid Checking
      else {
        write(connfd, "Invalid Command", sizeof("Invalid Command"));
      }

      break;
    }

    if (strcmp(EXIT, "BYEEE") == 0) {
      break;
    } else {

      memset( & Command, 0, sizeof(Command));
    }
    }

  

}

// Driver function
int main(int argc, char * argv[]) {
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero( & servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[3]));

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA * ) & servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    exit(0);
  } else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  } else
    printf("Server listening..\n");
  len = sizeof(cli);

  int kk = 1;
  timeout = atoi(argv[2]);
  int max_Client = atoi(argv[1]);
  while (1) {
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA * ) & cli, & len);
    if (connfd < 0) {
      printf("server accept failed...\n");

    } else {

      if (kk > max_Client) {
        write(connfd, "Clients Limit Reached", sizeof("Clients Limit Reached"));
      } else {

        pthread_t thread_id;
        pthread_create( & thread_id, NULL, MyThread, & connfd);
        kk = kk + 1;
      }

    }

  }

  close(sockfd);
}
