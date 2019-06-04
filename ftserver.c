#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFER_SIZE 70000


/*****************************************************
* Function: send_all()
* Description: funcion used to send all data to socket
******************************************************/
bool send_all(int socket, void *buffer, size_t length){
    char *ptr = (char*) buffer;
    // printf("SERVER: send_all(): length: %d\n", length);
    while (length > 0){
      int i = send(socket, ptr, length, 0);
      // printf("SERVER: sent %d bytes\n", i);
      if (i < 1) return false;
      ptr += i;
      length -= i;
    }
    return true;
}

/*****************************************************
* Function: receive_all()
* Description: funcion used to receive all data from socket
******************************************************/
bool receive_all(int socket, void* buffer){
  char* ptr = (char*) buffer;
  size_t len = sizeof(buffer);
  ssize_t count;
  while (len > 0 && (count = recv(socket, ptr, len, 0)) > 0){
    ptr += count;
    len =- (size_t)count;
  }
  if (len < 0 || count < 0){
    perror("SERVER: error in recv()\n");
  }
}

/*****************************************************
* Function: get_in_addr
* Description: returns sockaddr, works for both IPv4 or Ipv6
* Taken from Beej's guide
******************************************************/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int create_new_connection(char* ip_address, int portNumber){
  struct addinfo hints;
  struct addinfo *p;
  memset(&hints, '\0', sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(NULL, portNumber, &hints, &servinfor)) != 0){
    fprintf(stderr, "server: (second connection) getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  int sockfd;
  if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket 2");
  }
  if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: connect second socket");
  }
  return sockfd;
}


void process_command(int new_fd){
  // get command from client
  char buffer[BUFFER_SIZE];
  memset(buffer, '\0', sizeof(buffer));
  int message_len;
  int portNum;
  char ip_address[100];
  memset(ip_address, '\0', sizeof(ip_address));
  // retrieving the ip address from client
  recv(new_fd, ip_address, sizeof(ip_address), 0);
  // sending confirmation
  send(new_fd, "Y", 1, 0);

  // receiving command from client
  message_len = recv(new_fd, buffer, sizeof(buffer), 0);
  // sending confirmation
  send(new_fd, "Y", 1, 0);

  char filename[100];
  memset(filename, '\0', sizeof(filename));
  char data_port[100];
  memset(data_port, '\0', sizeof(data_port));
  char command[10];
  memset (command, '\0', sizeof(command));

  // process commands
  // ******************* case for -l command **************************
  if (strncmp(buffer, "-l", 2) == 0){
    strcpy(command, "list");
    // retrieving port number from buffer
    int i;
    for (i = 2; i < message_len; i++){
      data_port[i - 2] = buffer[i];
    }
    portNum = atoi(data_port);
    // creating new connection with data port number
    int data_socket = create_new_connection(ip_address, portNum);

    // copies files from current directory and stores them in a string
    DIR* d;
    struct dirent* directory;
    file_names[BUFFER_SIZE];
    d = opendir(".");
    int i = 0;
    int marker = 0;
    if (d){
      while ((dir = readdir(d)) != NULL){
        if (fir->d_type == DT_REG){
          strcpy(files_names, dir->d_name);
          i++;
        }
      }
      closedir(d);
    }
    else {
      perror("server: error opening directory\n");
    }
    size_t file_names_len = strlen(file_names);
    send_all(data_socket, file_names, file_names_len);
  }

  // ********************** case for -g command ***********************
  else if (strncmp(buffer, "-g", 2) == 0){
    strcpy(command, "get");
    // retrieving file name

    // get file name length

    // check if file name exists in directory

    // open file and store it in a buffer

    // get length of buffer

    // retrieving port number from buffer

    // creating new connection with data port number
    int data_socket = create_new_connection(ip_address, portNum);
    // send using send all

  }
  // *********************** case for an invalid command ***************
  else {
    // invalid command branch
    send(new_fd, "I", 1, 0);
  }
  // get confirmation

  // close second connection

}


int main(int argc, char* argv[]){
  if (argc != 2){
    perror("ERROR: Wrong number of command line parameters given\n");
    exit(1);
  }

  int portNumber = argv[1];

  struct addrinfo hints, *servinfo;
  int status;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, portNumber, &hints, &servinfor)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  int sockfd;
  struct addrinfo *p;

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
          perror("server: socket");
          continue;
      }

      // allows the socket to be reusable
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
          perror("setsockopt");
          exit(1);
      }

      // binds socket
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          close(sockfd);
          perror("server: bind");
          continue;
      }

      break;
  }
  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
  }

  // flip the socket on - it can now receive up to 5 connections
  if (listen(sockfd, 5) == -1) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  int new_fd;

  // starts endless loop to accept multiple connections
  while (1){
    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1){
      perror("ERROR: accept\n");
      continue;
    }
    else {
      int childExitMethod = -5;
      pid_t childPid = -5;
      pid_t spawnPid = -5;
      spawnPid = fork();
      switch(spawnPid){
        case -1:
          perror("ERROR: fork failed\n");
          close(new_fd);
        case 0:
          // This is the child process
          char buffer[BUFFER_SIZE];
          memset(buffer, '\0', sizeof(buffer));
          //sending confirmation to client
          send(new_fd, "Y", 1, 0);
          // receiving confirmation from client
          recv(new_fd, buffer, 1, 0);

          if (strncmp(buffer, "Y", 1) != 0){
            perror("server: error in handshake\n");
          }
          // sending confirmation
          send(new_fd, "Y", 1, 0);
          process_command(new_fd);

        default:
          // This is the parent process
          childPid = waitpid(-1, &childExitMethod, WNOHANG);
          close(new_fd);
      }
    }
  }

close(sockfd);
return 0;

}
