#include "Server.hpp"
#include "Channel.hpp"

#define RESET_COLOR "\033[0m"

#define BLACK "\033[30m"                    /* Black */
#define RED "\033[31m"                      /* Red */
#define GREEN "\033[32m"                    /* Green */
#define YELLOW "\033[33m"                   /* Yellow */
#define BLUE "\033[34m"                     /* Blue */
#define MAGENTA "\033[35m"                  /* Magenta */
#define CYAN "\033[36m"                     /* Cyan */
#define WHITE "\033[37m"                    /* White */
#define BOLD_BLACK "\033[1m\033[30m"        /* Bold Black */
#define BOLD_RED "\033[1m\033[31m"          /* Bold Red */
#define BOLD_GREEN "\033[1m\033[32m"        /* Bold Green */
#define BOLD_YELLOW "\033[1m\033[33m"       /* Bold Yellow */
#define BOLD_BLUE "\033[1m\033[34m"         /* Bold Blue */
#define BOLD_MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLD_CYAN "\033[1m\033[36m"         /* Bold Cyan */
#define BOLD_WHITE "\033[1m\033[37m"        /* Bold White */

using namespace std;

// constructor



Server::Server(int port, string password) : _port(port), _password(password) {
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": Port = " << port << "." << endl;
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": IP = localhost" << endl;
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": Password = ";
    for (size_t i = 0; i < password.size(); i++) cout << "*";
    cout << "." << endl;
    _nb_clients = 0;
    _password_match = false;
}

// destructor

Server::~Server() {

}

// other

void    Server::Cold_Start(void) {
    cout << BOLD_GREEN << "IRCSERV" << RESET_COLOR << ": Server cold start..." << endl; // Cold start :p
    struct sockaddr_in6 addr;

    this->_listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (this->_listen_fd < 0)
        throw runtime_error("Can't open the socket"); // Si _listen_fd est < 0 cela veut dire que socket fait mal son taff ce fou.
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket open..." << endl;
    
	this->setNonBlocking(this->_listen_fd);
    
    int opt = 1;
    if (setsockopt(this->_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
        throw runtime_error("Can't set option socket.");
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket options set..." << endl;

	const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
	addr.sin6_family = AF_INET6;
	addr.sin6_scope_id = 0;
	addr.sin6_flowinfo = 0;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(this->_port);
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": IPV6 addr set..." << endl;

    if (bind(this->_listen_fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(this->_listen_fd);
        throw runtime_error("Can't bind socket.");
    }
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket bind success..." << endl;
    
    if ((listen(_listen_fd, 256)) < 0) throw runtime_error("Can't listen socket.");
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Listen socket success..." << endl;

	cout << BOLD_CYAN << "Wait connection..." << RESET_COLOR << endl;


    struct vector<pollfd>   poll_struct;
    struct pollfd listen_fd_struct;
    listen_fd_struct.fd = this->_listen_fd;
    listen_fd_struct.events = POLLIN;
    listen_fd_struct.revents = 0;

    poll_struct.push_back(listen_fd_struct);

    map<int, User> users; // Map pour associer les sockets aux utilisateurs
    map<int, Channel> channels;
    while (1) {
        int pl = poll(poll_struct.data(), poll_struct.size(), -1);
        if (pl < 0) throw runtime_error("Can't read socket activity");

        for (size_t i = 0; i < poll_struct.size(); i++) {
            if (poll_struct[i].revents & POLLIN) {
                if (poll_struct[i].fd == this->_listen_fd) {
                    // Accepter une nouvelle connexion
                    int new_socket = accept(this->_listen_fd, NULL, NULL);
                    if (new_socket < 0) {
                        cerr << "Erreur lors de l'acceptation de la nouvelle connexion." << endl;
                        continue;
                    }
                    struct pollfd new_client;
                    new_client.fd = new_socket;
                    new_client.events = POLLIN;
                    new_client.revents = 0;
                    poll_struct.push_back(new_client);
                    users[new_socket] = User(); // Ajouter un nouvel utilisateur
                } else {
                    // Gérer l'entrée d'un client existant
                    handle_input(poll_struct[i].fd, users, channels);
                }
            }
        }
    }
}

void Server::handle_input(int client_socket, map<int, User>& users, map<int, Channel>& chanels) {
    char buf[1024];
    memset(buf, 0, sizeof(buf)); // Assurez-vous que le buffer est vide
    int bytesReceived = recv(client_socket, buf, sizeof(buf), 0);
    if (bytesReceived <= 0) {
        // Gestion de la déconnexion ou des erreurs
        close(client_socket);
        return;
    }

    // Log des données brutes reçues

    string clientMsg(buf, bytesReceived);
    istringstream iss(clientMsg);
    string command;

    while (getline(iss, command)) {
        vector<string> tokens;
        istringstream cmdStream(command);
        string token;
        while (cmdStream >> token) {
            tokens.push_back(token);
        }
        if (!tokens.empty()) {
            command_handler(tokens[0], client_socket, tokens, users, chanels);
        }
    }
}

void Server::command_handler(const string& command, int client_socket, const vector<string>& tokens, map<int, User>& users, map<int, Channel> channels) {
    (void) channels;
    if (command == "PASS") {
        handle_pass(client_socket, tokens, users);
    }
    else if (command == "NICK") {
        handle_nick(client_socket, tokens[1], users);
    }
    else if (command == "USER") {
        handle_user(client_socket, tokens, users);
    }
    else if (command == "CAP" && tokens.size() >= 2) {
        if (tokens[1] == "REQ") {
            // handle_cap_req(client_socket, tokens, users);
        } else if (tokens[1] == "LS") {
            // handle_cap_ls(client_socket, users);
        } else if (tokens[1] == "END") {
            // handle_cap_end(client_socket, users);
        }
    }
    else if (command == "PING") {
        handle_ping(client_socket, tokens[1]);
    }
    // else if (command == "JOIN") {
    //     handle_join(client_socket, tokens[1], users, channels);
    // }
    else if (command == "PRIVMSG")
        handle_privmsg(client_socket, tokens, users);
    else if (command == "QUIT") {
        handle_quit(client_socket, users);
    }
    else if (command == "motd")
        sendMOTD(client_socket, users);
    
    // Ajoutez d'autres commandes ici
}

void    Server::handle_privmsg(int client_socket, const vector<string>& tokens, map<int, User>& users) {
    map<int, User>::iterator it = users.find(client_socket);
    
    if (it != users.end()) {
        if (tokens[0] == "#") {
            // broadcast_channel(client_socket, tokens, users);

        }
    }
}

// void    Server::handle_join(int client_socket, const string& input, map<int, User>& users, map<int, Channel>& chanels) {

//     // map<int, User>::iterator it = users.find(client_socket);

//     // if (it != users.end()) {
//     //     ;
//     // }
//     // int i = 0;
//     // for (map<int, Channel>::iterator it : chanels) {
//     //     // if (chanel._fds == chanels._fds)
//     //     //     cout << "le chanel existe deja" << endl;
//     //     i++;
//     // }
//     // // auto chanel_create = chanels.find(input);
//     // cout << "nouveux chanel crée" << endl;
// }

void Server::handle_cap_ls(int client_socket, map<int, User>& users) {
    (void) users;
    string capResponse = ":localhost CAP * LS :multi-prefix sasl\r\n";
    send(client_socket, capResponse.c_str(), capResponse.length(), 0);
}

void Server::handle_cap_req(int client_socket, const vector<string>& tokens, map<int, User>& users) {
    // Exemple pour gérer une requête de capacité
    (void) users;
    string capability = tokens[2]; // Supposons que tokens[2] contient la capacité demandée
    if (capability == "multi-prefix") {
        string ackResponse = ":localhost CAP * ACK :multi-prefix\r\n";
        send(client_socket, ackResponse.c_str(), ackResponse.length(), 0);
    } else {
        string nakResponse = ":localhost CAP * NAK :" + capability + "\r\n";
        send(client_socket, nakResponse.c_str(), nakResponse.length(), 0);
    }
}

// void Server::handle_cap_end(int client_socket, map<int, User>& users) {
//     // La logique pour terminer la négociation CAP, si nécessaire.

// }


void Server::handle_pass(int client_socket, const vector<string>& tokens, map<int, User>& users) {
    // Supposons qu'un utilisateur n'a pas encore été ajouté à la map users ici
    if (tokens.size() < 2) {
        // Envoyer une réponse d'erreur si le mot de passe n'est pas fourni
        string errMsg = ":localhost 461 * PASS :Not enough parameters\r\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        cout << BOLD_RED << "IRCSERV" << RESET_COLOR << ": Pas assez d'argument, sur le client fd " << client_socket << endl;
        close(client_socket);
        return;
    }
    
    map<int, User>::iterator it = users.find(client_socket);
    if (it != users.end()) {
        if (tokens[1] == _password) {
            it->second.passReceived = true;
        } else {
            string errMsg = ":localhost 464 : Password incorrect\r\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            close(client_socket);
            cout << BOLD_RED << "IRCSERV" << RESET_COLOR << ": Password incorrect sur client fd " << client_socket << endl;
        }
    }
}

void Server::handle_nick(int client_socket, const string& newNick, map<int, User>& users) {
    // Iterate through users using traditional iterator
    map<int, User>::iterator it;
    for (it = users.begin(); it != users.end(); ++it) {
        if (it->second.get_nickname() == newNick && it->first != client_socket) {
            string errMsg = ":localhost 433 * " + newNick + " :Nickname is already in use...\r\n";
            send(client_socket, errMsg.c_str(), errMsg.length(), 0);
            return;
        }
    }

    // Check user existence and update accordingly
    it = users.find(client_socket);
    if (it == users.end()) {
        // If user doesn't exist, create new user with newNick
        User newUser;
        newUser.set_nickname(newNick);
        users[client_socket] = newUser;
        it->second.userReceived = true; // Assume this flag becomes true upon creation
    } else {
        // If user exists, update nickname and send messages
        string oldNick = it->second.get_nickname();
        it->second.set_nickname(newNick);

        string nickChangeMsg = ":" + oldNick + "!" + it->second.get_username() + "@" + it->second.get_hostname() + " NICK :" + newNick + "\r\n";
        send(client_socket, nickChangeMsg.c_str(), nickChangeMsg.length(), 0);
        it->second.nickReceived = true; // Update flag

        string nickChangeMsgOthers = "The client " + oldNick + ", changed his name to " + newNick + "\r\n";
        for (map<int, User>::iterator other_it = users.begin(); other_it != users.end(); ++other_it) {
            if (other_it->first != client_socket) { // Don't send to self
                send(other_it->first, nickChangeMsgOthers.c_str(), nickChangeMsgOthers.length(), 0);
            }
        }
    }
}


void Server::handle_user(int client_socket, const vector<string>& tokens, map<int, User>& users) {
    if (tokens.size() < 5) {
        string errMsg = ":localhost 461 * USER :Not enough parameters\r\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        return;
    }

    map<int, User>::iterator it = users.find(client_socket);
    if (it != users.end()) {
        it->second.set_username(tokens[1]);
        it->second.set_hostname(tokens[2]);
        it->second.set_servername(tokens[3]);
        it->second.set_realname(tokens[4]);
        it->second.userReceived = true;

        if (it->second.nickReceived && it->second.userReceived) {
            string welcomeMsg = ":localhost 001 " + it->second.get_nickname() + " :Welcome to the (best) FT_IRC network, " + it->second.get_nickname() + "\r\n";
            send(client_socket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
            cout << BOLD_GREEN << "IRCSERV" << RESET_COLOR << ": L'utilisateur avec le nick \"" << it->second.get_nickname() << "\" est maintenant connécté sur le serveur !" << endl;
            sendMOTD(client_socket, users);
        }
    }
}

void Server::sendMOTD(int client_socket, map<int, User>& users) {
    map<int, User>::iterator it = users.find(client_socket);

    // Début du MOTD
    string StartMOTDMsg = ":" + it->second.get_hostname() + " 375 " + it->second.get_nickname() + " :- FT_IRC Message of the Day - \r\n";
    send(client_socket, StartMOTDMsg.c_str(), StartMOTDMsg.length(), 0);

    // Corps du MOTD, envoyez autant de lignes 372 que nécessaire
    sendMOTDLine(client_socket, "- Welcome to Evan and Renaud FT_IRC Network", it->second.get_nickname(), it->second.get_hostname());
    sendMOTDLine(client_socket, "   Rules:", it->second.get_nickname(), it->second.get_hostname());
    sendMOTDLine(client_socket, "           o No spamming or flooding", it->second.get_nickname(), it->second.get_hostname());
    sendMOTDLine(client_socket, "           o No clones or malicious bots", it->second.get_nickname(), it->second.get_hostname());
    sendMOTDLine(client_socket, "           o No explicits or non-legal contains", it->second.get_nickname(), it->second.get_hostname());
    sendMOTDLine(client_socket, "- Now that you're warned, feel free to join your first channel. Enjoy ​😎​!", it->second.get_nickname(), it->second.get_hostname());
    // Ajoutez plus de règles ou d'informations ici
    sendNotice(client_socket, "*** Looking up your hostname...",  it->second.get_hostname());
    sendNotice(client_socket, "*** Checking Ident",  it->second.get_hostname());
    sendNotice(client_socket, "*** Couldn't look up your hostname",  it->second.get_hostname());
    sendNotice(client_socket, "*** No Ident response",  it->second.get_hostname());

    // Fin du MOTD
    string EndMOTDMsg = ":" + it->second.get_hostname() +" 376 " + it->second.get_nickname() + " :End of /MOTD command.\r\n";
    send(client_socket, EndMOTDMsg.c_str(), EndMOTDMsg.length(), 0);
}

void Server::sendNotice(int client_socket, const string& message, const string& Hostname) {
    string notice = ":" + Hostname + " NOTICE * :" + message + "\r\n";
    send(client_socket, notice.c_str(), notice.length(), 0);
}

void Server::sendMOTDLine(int client_socket, const string& message, const string& Nickname, const string& Hostname) {
    string motdLine = ":" +  Hostname + " 372 " + Nickname + " :" + message + "\r\n";
    send(client_socket, motdLine.c_str(), motdLine.length(), 0);
}


void Server::handle_ping(int client_socket, const string& input) {
    string pongResponse = "PONG " + input + "\r\n";
    send(client_socket, pongResponse.c_str(), pongResponse.length(), 0);
}

void Server::handle_quit(int client_socket, map<int, User>& users) {
    map<int, User>::iterator it = users.find(client_socket);
    string goodbyeMsg = ":localhost QUIT :Client has quit\r\n";
    send(client_socket, goodbyeMsg.c_str(), goodbyeMsg.length(), 0);
    cout << BOLD_YELLOW << "IRCSERV" << RESET_COLOR << ": L'utilisateur avec le nick \"" << it->second.get_nickname() << "\" est maintenant deconnécté du serveur !" << endl;
    // Fermez la connexion socket
    close(client_socket);

    // Supprimez l'utilisateur de la map
    users.erase(client_socket);
}

// void    Server::command_not_found(int client_socket, string command) {
//     string notFoundMsg = "421 evan :The command  :" + command + " does not exist \r\n";
    
//     if (send(client_socket, notFoundMsg.c_str(), notFoundMsg.size(), 0) <= 0) {
//         cerr << "Error message de unknowncomamnd (421)" << endl;
//         return ;
//     }
// }


void Server::setNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		throw runtime_error("Can't get flags for socket.");
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		throw runtime_error("Can't set socket to non-blocking.");
	}
}
