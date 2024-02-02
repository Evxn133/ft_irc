#ifndef USER_HPP
#define USER_HPP


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

#include "Server.hpp"

using namespace std;

class User {
    public:
        User(void);
        ~User(void);
        void    set_nickname(string nick_received);
        void    set_client_fd(int fd_received);
        string    get_nickname(void);
        int    get_client_fd(void);
        friend ostream& operator<<(ostream& os, const User& user);
    private:
        string  _nick;
        string  _user;
        int     _client_fd;
};

#endif