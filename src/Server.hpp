#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;

class Server {
    public:
        Server(int port, string password);
        ~Server();
        void    Cold_Start(void);
        void    setNonBlocking(int sock);
    private:
        int _listen_fd;
        int _port;
        string  _password;
};

#endif