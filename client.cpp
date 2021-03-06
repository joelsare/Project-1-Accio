#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <netdb.h>
#include <chrono>

using namespace std; 


int main(int argc, char *args[])
{
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  char * hostName = args[1];

  stringstream p(args[2]);
  int port;
  p >> port;

  if ((port <= 1023))
  {
    std::cerr << "ERROR: invalid arguments" << std::endl;
    return 1;
  }

  if (gethostbyname(hostName) == NULL)
  {
    std::cerr << "ERROR: invalid arguments" << std::endl;
    return 1;
  }

  string fileName = args[3];

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr(hostName);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
  {
    perror("connect");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1)
  {
    perror("getsockname");
    return 3;
  }

  if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
    cerr << "Error";
    return 1;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));


  // send/receive data to/from connection
  char buf[20] = {0};
  std::stringstream ss;

  while (1)
  {
    memset(buf, '\0', sizeof(buf));

    if (send(sockfd, fileName.c_str(), fileName.size(), 0) == -1)
    {
      //perror("send");
      //return 4;
    }

    if (recv(sockfd, buf, 20, 0) == -1)
    {
      //perror("recv");
      //return 5;
    }
    ss << buf << std::endl;

    if (ss.str() == "close\n")
    {
      break;
    }
    ss.str("");
  }
  close(sockfd);

  return 0;
}