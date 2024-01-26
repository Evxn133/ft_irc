#include "server.hpp"

// constructor

Server::Server(char **av) {
	_server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_server_fd < 0) {
		perror("Échec de la création du socket");
		exit(EXIT_FAILURE);
	}
	_opt = 1;
	_name = "HIER C";
	_nb_user = 0;
	_password = av[2];
	_port = std::atoi(av[1]);
	_client_fds.clear();
	_server_socket = 0;
	std::cout << "server '" << _name << "' created" << std::endl << std::endl;
}

Server::Server(const Server& other) {

}

// destructor

Server::~Server(void) {
	// delete [] _users;
}

// operator

Server &Server::operator=(const Server& other) {
	return *this;
}

// other

// void Server::addUser(User& new_user) {
//     if (!_users) {
//         _nb_user = 1;
//         _users = new User[1];
//         _users[0] = new_user;
//     } else {
//         _nb_user++;
//         User *temp = new User[_nb_user];
//         int i = 0;
//         while (i < _nb_user--) {
//             temp[i] = _users[i];
//             i++;
//         }
//         temp[i] = new_user;
//         _users = temp;
//     }
// }

// void Server::printUser() {
//     int i = 0;
//     std::cout << "nb user = " << _nb_user << std::endl;
//     while (i < _nb_user) {
//         std::cout << i << ": user nick name = " << _users[i].getNickName() << ", id = " << _users[i].getId() << std::endl;
//         i++;
//     }
// }

void Server::listen(void) {
	struct sockaddr_in6 addr;

	this->_server_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (this->_server_socket == 0)
		throw std::runtime_error("Error: Can't create socket.");

	int opt = 1;
	if (setsockopt(this->_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error: Can't set option socket.");

	this->setNonBlocking(this->_server_socket);

	const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
	addr.sin6_family = AF_INET6;
	addr.sin6_scope_id = 0;
	addr.sin6_flowinfo = 0;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(this->_port);

	if (bind(this->_server_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		close(this->_server_socket);
		throw std::runtime_error("Error: Can't connect socket.");
	}

 	std::cout << "Start irc_server on " << this->_port << std::endl;

	if (::listen(this->_server_socket, 32) < 0)
		throw std::runtime_error("Error: Can't listen socket.");

	this->_constructFds();
	std::cout << "Waiting connection..." << std::endl;
	while (1)
		this->_wait_connection();
}


void Server::setNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		throw std::runtime_error("Error: Can't get flags for socket.");
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		throw std::runtime_error("Error: Can't set socket to non-blocking.");
	}
}

void Server::_constructFds(void) {
    // Remplacez 'this->_clients' par 'this->_user'
    std::vector<struct pollfd> temp_clients_fds(this->_user.size() + 1);

    temp_clients_fds[0].fd = this->_server_socket;
    temp_clients_fds[0].events = POLLIN;

    for (unsigned long i = 0; i < this->_user.size(); i++) {
        // Assurez-vous que la classe User a une méthode getFD()
        temp_clients_fds[i + 1].fd = this->_user[i]->getFD();
        temp_clients_fds[i + 1].events = POLLIN;
    }

    // Échanger notre vector temporaire avec le membre _client_fds
    std::swap(this->_client_fds, temp_clients_fds);
}

void Server::_wait_connection(void) {
    int pl = poll(this->_client_fds.data(), this->_client_fds.size(), -1);

    if (pl < 0) {  // si poll renvoie -1, cela veut dire qu'il ne peut lire le socket.
        std::cout << "Irc server error: Can't read socket activity..." << std::endl;
        return; // Ajout d'un return pour arrêter l'exécution en cas d'erreur.
    }

    for (size_t i = 0; i < this->_client_fds.size(); i++) {
        if (this->_client_fds[i].revents & POLLIN) {
            if (this->_client_fds[i].fd == this->_server_socket) {
                this->acceptNewConnections();
            }
			else {
                User *user = this->_user[i - 1];
                // this->_receivedata(client);
            }
        }
    }
}

void Server::acceptNewConnections() {
    int newSocketFD;
    struct sockaddr_in6 clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    while (true) {
        newSocketFD = accept(this->_server_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        
        // Vérifier si l'acceptation a échoué
        if (newSocketFD < 0) {
            // Sortir de la boucle si on ne peut pas accepter de nouvelles connexions
            if (errno != EWOULDBLOCK) {
                std::cerr << "Error: Unable to accept new connection." << std::endl;
            }
            break;
        }
        
        // Ajouter la nouvelle connexion à la gestion des utilisateurs
        handleNewUser(newSocketFD, clientAddr); // Vous devez implémenter cette fonction selon votre logique de gestion des utilisateurs
        std::cout << "New client connected." << std::endl;
    }
}

void Server::handleNewUser(int socketFD, const sockaddr_in6& clientAddr) {
    // Définir le socket comme non bloquant
    setNonBlocking(socketFD);

    // Ajouter le nouvel utilisateur à la liste des utilisateurs gérés par le serveur
    // Par exemple: this->_user.push_back(new User(socketFD, ...));
    // ...
    
    // Mettre à jour la liste des descripteurs de fichier pour poll
    _constructFds();
}
