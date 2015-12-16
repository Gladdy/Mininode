#include "tcpserver.h"

#include "data.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <netdb.h>
#include <sys/fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define BACKLOG 10

TCPServer::TCPServer(std::function<void(Socket&)> sf)
{
  socketFunction = sf;
}

void TCPServer::block() {
  while(activeCounter) {
    usleep(10000);
  }
}

void TCPServer::listen(uint16_t p) {
  activeCounter++;
  port = p;
  acceptingThread = std::thread(&TCPServer::acceptConnections, this);
  processingThread = std::thread(&TCPServer::processConnections, this);
}

void TCPServer::acceptConnections() {

  int sockfd;

  struct addrinfo hints, *servinfo;
  struct sockaddr remote_address;

  socklen_t remote_address_size = sizeof(remote_address);
  int yes = 1;
  int rv;
  char portbuffer[6];
  char s[INET_ADDRSTRLEN];


  //Fill in the parameters for the socket to listen on for new connections
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;        // AF_UNSPEC (both), AF_INET (v4), AF_INET6 (v6)
  hints.ai_socktype = SOCK_STREAM;    // Stream socket: TCP
  hints.ai_flags = AI_PASSIVE;        // Listen on my own address


  //Get the address on which we can listen (our own address)
  snprintf (portbuffer, 6, "%d", port);
  if ((rv = getaddrinfo(NULL, portbuffer, &hints, &servinfo)) != 0) {
    std::cerr << "Unable to get listening address: " << gai_strerror(rv) << std::endl;
    return;
  }

  //Create a socket
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
    std::cerr << "Unable to create listening socket: " << strerror(errno) << std::endl;
    return;
  }

  //Open the socket
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    close(sockfd);
    std::cerr << "Unable to open listening socket: " << strerror(errno) << std::endl;
    return;
  }

  //Bind the created and opened socket to the address from getaddrinfo()
  if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
    close(sockfd);
    std::cerr << "Unable to bind to listening socket: " << strerror(errno) << std::endl;
    return;
  }

  //Start listening to the socket
  if (::listen(sockfd, BACKLOG) == -1) {
    close(sockfd);
    std::cerr << "Too many connection requests: " << strerror(errno) << std::endl;
    return;
  }

  std::cout << "Finished initializing the listening socket" << std::endl;

  while(true)
  {
    //Accept a new connection (this call blocks)

    //This call blocks
    //Store the new connection in the remote_address sockaddr struct
    int new_fd = accept(sockfd, &remote_address, &remote_address_size);
    struct sockaddr_in * remote_data = (struct sockaddr_in *)&remote_address;

    uint16_t port = ntohs(remote_data->sin_port);
    const char * hostname = inet_ntop(AF_INET, &(remote_data->sin_addr), s, sizeof(s));

    if (new_fd == -1) {
      std::cerr << "Unable to accept connection: " << strerror(errno) << std::endl;
      continue;
    }

    // Set the socket to be non-blocking
    if(fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1) {
      std::cerr << "Unable to set socket to non blocking: " << strerror(errno) << std::endl;
      close(new_fd);
      continue;
    }

    //Execute the connection function
    //Create a C++ socket object
    //Hand it to the socket storage for polling / processing
    Socket accepted_socket (new_fd, hostname, std::to_string(port));
    socketVector.push_back(accepted_socket);

    socketFunction(socketVector.back());
  }
}

void TCPServer::processConnections() {

  std::cout << "Processing connections" << std::endl;
  unsigned waittime = 1;
  unsigned maxwaittime = 100000;

  while(true) {

    for(unsigned i = 0; i < socketVector.size(); i++) {

      Socket& s = socketVector.at(i);

      ssize_t received = recv(s.fileDescriptor, s.buffer, sizeof(s.buffer), 0);

      if(received == 0) {
        //Socket closed
        s.endFunction();
        close(s.fileDescriptor);
        socketVector.erase(socketVector.begin()+i);

        waittime = 0;
      }
      else if(received != -1) {
        // Socket received data

        Data d (std::string(s.buffer, received));
        s.dataFunction(d);
        waittime = 0;
      } else {
        // Increase the wait period up to a maximum
        waittime = std::min(waittime * 2, maxwaittime);
      }

      usleep(waittime);
    }

    if(socketVector.size() == 0) {
      usleep(maxwaittime);
    }

  }
}

uint32_t TCPServer::activeCounter = 0;
