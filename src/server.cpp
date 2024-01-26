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
	_client_fds = 0;
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

	this->_constructorFds();
	std::cout << "Waiting connection..." << std::endl;
	while (1)
	{
		// this->_wait_connection();
	}
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

void Server::_constructorFds() {
	// Initialiser les variables ou structures nécessaires pour votre serveur
	this->_client_fds = -1; // Supposons que c'est un identifiant de descripteur de fichier pour le dernier client accepté

	// Si vous utilisez `poll` ou `select`, vous pouvez initialiser des structures `pollfd` ou des ensembles `fd_set` ici
	// Exemple pour `poll`:
	std::vector<struct pollfd> fds;
	struct pollfd server_fd_data;
	server_fd_data.fd = this->_server_socket;  // Le socket du serveur
	server_fd_data.events = POLLIN;            // On attend des données en lecture (des connexions entrantes)
	fds.push_back(server_fd_data);

	// La structure `fds` peut être un membre de votre classe `Server` si vous souhaitez y accéder ailleurs
}

void	Server::_wait_connection(void) {
	int pl = poll(this->_clients_fds, this->_clients.size() + 1, - 1)

	if (rc < 0 ) // si poll renvoie -1, cela veut dire qu'il ne peut lire le socket.
		std::cout << "Irc server error: Can't read socket activity..." << std::endl;

	for (i = 0; i < this->_client.size() + 1; i++)
	dd
}