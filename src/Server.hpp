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
#include <cstdlib>

#include "User.hpp"

using namespace std;

class User;

class Server {

    public:

        Server(int port, string password);
        ~Server();

        void    Cold_Start(void);
        void    setNonBlocking(int sock);
        void    Welcome_Message(int client_socket);
        void handle_input(int client_socket, unordered_map<int, User>& users);
        void    command_not_found(int client_socket, string command);
    
    private:
        vector<User> _users;
        int     _listen_fd;
        int     _port;
        int     _nb_clients;
        bool    _bool_cmd;
        string  _password;
};

#endif