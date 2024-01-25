#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>

#include "user.hpp"

class Server {
	public:
		Server(char **av);
		Server(const Server& other);
		~Server(void);

		Server& operator=(const Server& other);

        void addUser(User& new_user);
        void printUser();
        void listen();

	private:
        std::string _name;
		int _nb_user;
        std::string _password;
        int _port;
        User _user;
        int _client_fds;
        int _server_fd;
        int _opt;
        int _server_socket;
};

#endif