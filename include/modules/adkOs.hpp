#include "aardvark.h"
#include <iomanip>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <regex>
//We should probably rename this module "socket"
using namespace Aardvark;
class AdkSocket {
  public:
  int socketfd;
  struct sockaddr_in addr;

  AdkSocket(int port) {
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
  }
  
  std::string recv(int sockfd) {
    char buffer[1024] = {0};

    ::recv(sockfd, &buffer, 1024, 0);

    std::string data(buffer);

    return data;
  }
  
  int bindSocket() {
    int err = bind(socketfd, (struct sockaddr*)&addr, sizeof(addr));

    return err;
  }

  void listen() {
    ::listen(socketfd, 5);
  }

  int accept() {
    struct sockaddr_in sockAddr;
    socklen_t addrsize = sizeof(sockAddr);
    int newsockfd = ::accept(socketfd, (struct sockaddr*)&sockAddr, &addrsize);

    if (newsockfd < 0) {
      close(newsockfd);
    }

    return newsockfd;
  }
  
  void close(int sockfd) {
    // std::cout << "Shutting Down Socket: " << sockfd << std::endl;
    shutdown(sockfd, 2);
    close(sockfd);
  }
};

AdkValue* initSocket(int port, Interpreter* i) {
  AdkObject* obj = new AdkObject("Socket");
  AdkSocket* socket = new AdkSocket(port);

  obj->Set("recv", new AdkFunction(
    i,
    [=](vector<AdkValue*> args, Interpreter* i2) {
      if (args.size() < 1 || args[0]->type != DataTypes::Int) {
        std::cout << "Error: Please specify a socket handle to recieve input from!" << std::endl;
        throw "err";
      }

      int fd = args[0]->toInt();
      std::string data = socket->recv(fd);

      return i2->ConstructString(data);

    }, "recv")
  );

  obj->Set("listen", new AdkFunction(
    i,
    [=](vector<AdkValue*> args, Interpreter* i2) {
      socket->listen();

      return new AdkValue();
    }, "listen")
  );

  obj->Set("bind", new AdkFunction(
    i,
    [=](vector<AdkValue*> args, Interpreter* i2) {
      return new AdkValue(socket->bindSocket());
    }, "bind")
  );

  obj->Set("accept", new AdkFunction(
    i,
    [=](vector<AdkValue*> args, Interpreter* i2) {
      return new AdkValue(socket->accept());
    }, "accept")
  );

  obj->Set("close", new AdkFunction(
    i,
    [=](vector<AdkValue*> args, Interpreter* i2) {
      if (args.size() < 1 || args[0]->type != DataTypes::Int) {
        std::cout << "Error: Please specify a socket handle to close!" << std::endl;
        throw "err";
      }

      int fd = args[0]->toInt();
      socket->close(fd);

      return new AdkValue();
    }, "close")
  );

  return obj;
}

AdkValue* initOsModule(Interpreter* i) {
  AdkObject* obj = new AdkObject("os");
  obj->Set("Socket", new AdkFunction(
    i,
    [](vector<AdkValue*> args, Interpreter* i2) {
      if (args.size() < 1 || args[0]->type != DataTypes::Int) {
        std::cout << "Error: Please specify a port!" << std::endl;
        throw "err";
      }

      int port = args[0]->toInt();

      return initSocket(port, i2);

    }, "Socket")
  );

  return (AdkValue*)obj;
}