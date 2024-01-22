#include "server.hpp"

// constructor

Server::Server(char **av) {
    _server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (_server_fd < 0) {
        perror("Échec de la création du socket");
        exit(EXIT_FAILURE);
    }
    _opt = 1;
	_name = "hier c'est";
    _nb_user = 0;
    _password = av[2];
    _port = std::atoi(av[1]);
    _users = NULL;
    std::cout << "server '" << _name << "' created" << std::endl << std::endl;
}

Server::Server(const Server& other) {

}

// destructor

Server::~Server(void) {
    delete [] _users;
}

// operator

Server &Server::operator=(const Server& other) {
	return *this;
}

// other

void Server::addUser(User& new_user) {
    if (!_users) {
        _nb_user = 1;
        _users = new User[1];
        _users[0] = new_user;
    } else {
        _nb_user++;
        User *temp = new User[_nb_user];
        int i = 0;
        while (i < _nb_user--) {
            temp[i] = _users[i];
            i++;
        }
        temp[i] = new_user;
        _users = temp;
    }
}

void Server::printUser() {
    int i = 0;
    std::cout << "nb user = " << _nb_user << std::endl;
    while (i < _nb_user) {
        std::cout << i << ": user nick name = " << _users[i].getNickName() << ", id = " << _users[i].getId() << std::endl;
        i++;
    }
}
