#include "server.hpp"

// constructor

#define DEBUG 1  // Mettre à 0 pour désactiver l'impression de débogage

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

    if (pl < 0) {
        std::cout << "Irc server error: Can't read socket activity, error: " << strerror(errno) << std::endl;
        return;
    }

    for (size_t i = 0; i < this->_client_fds.size(); i++) {
        if (this->_client_fds[i].revents & POLLIN) {
            if (this->_client_fds[i].fd == this->_server_socket) {
                std::cout << "New connection on server socket" << std::endl;
                this->acceptNewConnections();
            } else {
                // Vérifier que l'index de l'utilisateur est valide
                size_t userIndex = i - 1;
                if (userIndex < this->_user.size()) {
                    User *user = this->_user[userIndex];
                    this->processClientInput(user);  // Traitement des données entrantes pour l'utilisateur
                } else {
                    std::cerr << "Invalid user index: " << userIndex << std::endl;
                }
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
        
        if (newSocketFD < 0) {
            // ... gestion des erreurs ...
            break;
        }
        
        // Ajouter la nouvelle connexion à la gestion des utilisateurs
        handleNewUser(newSocketFD, clientAddr);
        _constructFds(); // Recréer le tableau _client_fds pour inclure le nouveau client
    }
}

void Server::handleNewUser(int socketFD, const sockaddr_in6& clientAddr) {
    std::vector<std::string> welcomeMessageLines = {
        "Bienvenue sur :",
        ".",
        ".",
        ".##.....##.####.########.########...........######.",
        ".##.....##..##..##.......##.....##.........##....##",
        ".##.....##..##..##.......##.....##.........##......",
        ".#########..##..######...########..........##......",
        ".##.....##..##..##.......##...##...........##......",
        ".##.....##..##..##.......##....##..........##....##",
        ".##.....##.####.########.##.....##.#######..######.",
        ".",
        "."
    };
    User* newUser = new User(socketFD);
    this->_user.push_back(newUser);
    
    // Envoyer le message de bienvenue
    for (const auto& line : welcomeMessageLines) {
        std::string messageToSend = "\033[31m" + line + "\033[0m\n";
        sendMessage(socketFD, messageToSend);
    }
}


void Server::sendMessage(int clientFD, const std::string& message) {
    // Envoyer le message au client spécifié par clientFD
    ssize_t bytesSent = send(clientFD, message.c_str(), message.length(), 0);
    if (bytesSent < 0) {
        // Gérer l'erreur d'envoi
        std::cerr << "Error: Unable to send message to fd " << clientFD << std::endl;
    } else if (bytesSent < static_cast<ssize_t>(message.length())) {
        // Gérer le cas où le message n'a pas été complètement envoyé
        std::cerr << "Warning: Partial message sent to fd " << clientFD << std::endl;
    }
}

void Server::processClientInput(User* user) {
    char dataBuffer[BUFFER_SIZE + 1];

    while (true) {
        int receivedBytes = recv(user->getFD(), dataBuffer, sizeof(dataBuffer) - 1, 0);

        if (receivedBytes < 0) {
            if (errno != EWOULDBLOCK) {
                std::cerr << "Error receiving data from fd " << user->getFD() << std::endl;
                removeUser(user->getFD());
            }
            break;
        } else if (receivedBytes == 0) {
            removeUser(user->getFD());
            break;
        } else {
            dataBuffer[receivedBytes] = '\0';
            std::string receivedData = dataBuffer;

            if (receivedData.back() == '\n') {
                handleCompleteMessages(user, receivedData);
            } else {
                user->appendToRecvBuffer(receivedData);
                if (DEBUG) {
                    std::cout << "Received partial data from fd " << user->getFD() << ": " << receivedData << std::endl;
                }
            }
        }
    }
}

void Server::handleCompleteMessages(User* user, const std::string& data) {
    std::vector<std::string> commands = splitCommands(user->getRecvBuffer() + data, '\n');
    user->clearRecvBuffer();
    
    for (const auto& cmd : commands) {
        processCommand(cmd, user);
    }
}

void Server::removeUser(int userFD) {
    // Trouvez et supprimez l'utilisateur ayant le file descriptor userFD
    for (auto it = _user.begin(); it != _user.end(); ++it) {
        if ((*it)->getFD() == userFD) {
            delete *it;  // Libérez la mémoire si vous avez alloué dynamiquement l'utilisateur
            _user.erase(it);  // Supprimez l'utilisateur de la liste
            break;
        }
    }
}

std::vector<std::string> Server::splitCommands(const std::string& data, char delimiter) {
    std::vector<std::string> commands;
    std::string command;
    std::istringstream stream(data);

    while (std::getline(stream, command, delimiter)) {
        commands.push_back(command);
    }

    return commands;
}

void Server::processCommand(const std::string& cmd, User* user) {
    // Découper la commande en mots
    std::istringstream iss(cmd);
    std::vector<std::string> words{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

    if (words.empty()) {
        return;
    }
    std::cout << "coucou" << std::endl;
    // Traiter les commandes
    if (words[0] == "NICK" && words.size() > 1) {
        // La commande est NICK, le client souhaite changer de surnom
        std::string newNick = words[1];
        user->setNickName(newNick);  // Assurez-vous que vous avez une méthode pour changer le surnom de l'utilisateur
        std::string ack = "Votre nouveau nick est " + newNick + "\n";
        std::cout << "Nouveau client avec comme nick : " << newNick << std::endl;
        sendMessage(user->getFD(), ack);
    }
    else if (words[0] == "MSG" && words.size() > 2) {
        std::string dest = words[1];
        std::string messageContent = cmd.substr(cmd.find(' ', cmd.find(' ') + 1) + 1);
        // Vérifiez si dest est un utilisateur
        User* destUser = getUserByNickName(dest);
        if (destUser) {
            sendMessage(destUser->getFD(), "Message de " + user->getNickName() + ": " + messageContent);
        } else {
            // Vérifiez si dest est un canal
            Channel* destChannel = getChannelByName(dest);
            if (destChannel) {
                // Envoyer le message à tous les membres du canal
                for (User* member : destChannel->getMembers()) {
                    sendMessage(member->getFD(), "Message dans " + dest + " de " + user->getNickName() + ": " + messageContent);
                }
            } else {
                sendMessage(user->getFD(), "Destination inconnue: " + dest);
            }
        }
    }
}


void User::setNickName(const std::string& nickName) {
    _nick_name = nickName;
}

User* Server::getUserByNickName(const std::string& nickName) {
    for (User* user : this->_user) {
        if (user->getNickName() == nickName) {
            return user;
        }
    }
    return nullptr; // Retourne nullptr si aucun utilisateur avec ce surnom n'est trouvé
}

Channel* Server::getChannelByName(const std::string& channelName) {
    for (Channel* channel : this->_channels) {
        if (channel->getName() == channelName) {
            return channel;
        }
    }
    return nullptr;
}