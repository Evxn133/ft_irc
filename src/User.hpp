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
        friend ostream& operator<<(ostream& os, const User& user);
        void    set_nickname(string nick_received);
        void    set_client_fd(int fd_received);
        void    set_username(string username_received);
        void    set_hostname(string hostname_received);
        void    set_realname(string realname_received);
        void    set_servername(string servername_received);
        int     get_client_fd(void) const;
        string    get_username(void) const;
        string    get_hostname(void) const;
        string    get_realname(void) const;
        string    get_servername(void) const;
        string    get_nickname(void) const;
        bool passReceived = false;
        bool userReceived = false;
    private:
        int     _client_fd;
        string  _nick;
        string  _user;
        string  _username;
        string  _hostname;
        string  _realname;
        string  _server_name;
};

#endif