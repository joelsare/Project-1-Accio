#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sstream> 
#include <thread>



#include <iostream>
#include <sstream>
using namespace std; 


int main(int argc, char *args[])
{
  stringstream p(args[1]);
  int port;
  p >> port;

  if ((port <= 1023))
  {
    std::cout << "ERROR: port must be above 1023" << std::endl;
    return 1;
  }
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(40000);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
  {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1)
  {
    perror("listen");
    return 3;
  }

  int pid;
  int new1;
  int counter = 0;

  while (1)
  {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    new1 = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (new1 == -1)
    {
      perror("accept");
      return 4;
    }

    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
    std::cout << "Accept a connection from: " << ipstr << ":" <<
      ntohs(clientAddr.sin_port) << std::endl;

    // read/write data from/into the connection
    char buf[20] = {0};
    std::stringstream ss;

    if((pid = fork()) == -1)
    {
      close(new1);
      continue;
    }
    else if (pid > 0)
    {
      close(new1);
      counter++;
      continue;
    }
    else if (pid == 0)
    {
      counter++;
      memset(buf, '\0', sizeof(buf));

      if (recv(new1, buf, 20, 0) == -1)
      {
        perror("recv");
        return 5;
      }

      ss << buf << std::endl;
      std::cout << buf << std::endl;

      if (send(new1, buf, 20, 0) == -1)
      {
        perror("send");
        return 6;
      }

      if (ss.str() == "close\n")
        break;

      ss.str("");
    }
  }

  close(new1);

  return 0;
}