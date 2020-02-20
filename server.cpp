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
#include <fstream>
#include <signal.h>
#include <chrono>


using namespace std; 

void signalHandler(int signum)
{
  exit(0);
}

int main(int argc, char *args[])
{
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  std::stringstream ss;
  char buf[20] = {0};
  stringstream p(args[1]);
  int port;
  int val;
  int new1;
  int counter = 0;
  string dirName = args[2];
  dirName = dirName.substr(1) + "/";

  signal(SIGQUIT, signalHandler);
  signal(SIGTERM, signalHandler);

  p >> port;
  if ((port <= 1023))
  {
    std::cerr << "ERROR: port must be above 1023" << std::endl;
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

  if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
    cerr << "Error";
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);     // short, network byte order
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
  while (1)
  {
    string fileName;
    string destination;

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    new1 = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);
    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));

    val = fork();
    if (new1 == -1)
    {
      if (counter == 0)
      {
        counter++;
      }
      string destination = dirName + std::to_string(counter) + ".file";
      std::ofstream file;
      file.open(destination);
      file << "ERROR";
      file.close();
      return 1;
    }

    if(val  == -1)
    {
      close(new1);
      continue;
    }
    else if (val > 0)
    {
      close(new1);
      counter++;
      continue;
    }
    else if (val == 0)
    {
      memset(buf, '\0', sizeof(buf));
      
      if (recv(new1, buf, 20, 0) == -1)
      {
        perror("recv");
        return 5;
      }

      counter++;
      ss << buf << std::endl;
      fileName = buf;
      destination = dirName + std::to_string(counter) + ".file";

      std::ifstream src(fileName);
      std::ofstream dst(destination);
      //cout << src.tellg();
      dst << src.rdbuf();

      if (send(new1, buf, 20, 0) == -1)
      {
        perror("send");
        return 6;
      }

      if (ss.str() == "close\n")
      {
        break;
      }

      ss.str("");
      close(new1);
      break;
    }
  }

  close(new1);

  return 0;
}
