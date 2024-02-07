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
    for (int i = 0; i < password.size(); i++) cout << "*";
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

    unordered_map<int, User> users; // Map pour associer les sockets aux utilisateurs
    while (1) {
        int pl = poll(poll_struct.data(), poll_struct.size(), -1);
        if (pl < 0) throw runtime_error("Can't read socket activity");

        for (int i = 0; i < poll_struct.size(); i++) {
            if (poll_struct[i].revents & POLLIN) {
                if (poll_struct[i].fd == this->_listen_fd) {
                    // Accepter une nouvelle connexion
                    int new_socket = accept(this->_listen_fd, NULL, NULL);
                    if (new_socket < 0) {
                        cerr << "Erreur lors de l'acceptation de la nouvelle connexion." << endl;
                        continue;
                    }
                    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Nouveau client sur fd " << new_socket << endl;
                    struct pollfd new_client;
                    new_client.fd = new_socket;
                    new_client.events = POLLIN;
                    new_client.revents = 0;
                    poll_struct.push_back(new_client);
                    users[new_socket] = User(); // Ajouter un nouvel utilisateur
                } else {
                    // Gérer l'entrée d'un client existant
                    handle_input(poll_struct[i].fd, users);
                }
            }
        }
    }
}

void Server::handle_input(int client_socket, unordered_map<int, User>& users) {
    char buf[1024];
    int bytesReceived = recv(client_socket, buf, 1024, 0);
    if (bytesReceived <= 0) {
        // Gestion de la déconnexion ou des erreurs
        return;
    }
    string clientMsg(buf, bytesReceived);
    istringstream iss(clientMsg);
    vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};
    if (!tokens.empty()) {
        const string& command = tokens[0];
        command_handler(command, client_socket, tokens, users);
    }
}

void Server::command_handler(const string& command, int client_socket, const vector<string>& tokens, unordered_map<int, User>& users) {
    if (command == "PASS") {
        handle_pass(client_socket, tokens, users);
    } else if (command == "NICK") {
        handle_nick(client_socket, tokens[1], users);
    } else if (command == "USER") {
        handle_user(client_socket, tokens, users);
    } else if (command == "CAP" && tokens.size() >= 2) {
        if (tokens[1] == "REQ") {
            // handle_cap_req(client_socket, tokens, users);
        } else if (tokens[1] == "LS") {
            // handle_cap_ls(client_socket, users);
        } else if (tokens[1] == "END") {
            // handle_cap_end(client_socket, users);
        }
    } else if (command == "PING") {
        handle_ping(client_socket, tokens[1]);
    } else if (command == "QUIT") {
        handle_quit(client_socket, users);
    }
    // Ajoutez d'autres commandes ici
}

void Server::handle_cap_ls(int client_socket, unordered_map<int, User>& users) {
    string capResponse = ":localhost CAP * LS :multi-prefix sasl\r\n";
    send(client_socket, capResponse.c_str(), capResponse.length(), 0);
}

void Server::handle_cap_req(int client_socket, const vector<string>& tokens, unordered_map<int, User>& users) {
    // Exemple pour gérer une requête de capacité
    string capability = tokens[2]; // Supposons que tokens[2] contient la capacité demandée
    if (capability == "multi-prefix") {
        string ackResponse = ":localhost CAP * ACK :multi-prefix\r\n";
        send(client_socket, ackResponse.c_str(), ackResponse.length(), 0);
    } else {
        string nakResponse = ":localhost CAP * NAK :" + capability + "\r\n";
        send(client_socket, nakResponse.c_str(), nakResponse.length(), 0);
    }
}

void Server::handle_cap_end(int client_socket, unordered_map<int, User>& users) {
    // La logique pour terminer la négociation CAP, si nécessaire.
}


void Server::handle_pass(int client_socket, const vector<string>& tokens, unordered_map<int, User>& users) {
    // Supposons qu'un utilisateur n'a pas encore été ajouté à la map users ici
    if (tokens.size() < 2) {
        // Envoyer une réponse d'erreur si le mot de passe n'est pas fourni
        string errMsg = ":localhost 461 * PASS :Not enough parameters\r\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        cout << BOLD_RED << "IRCSERV" << RESET_COLOR << ": Pas assez d'argument, sur le client fd " << client_socket << endl;
        close(client_socket);
        return;
    }
    
    auto it = users.find(client_socket);
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

void Server::handle_nick(int client_socket, const string& newNick, unordered_map<int, User>& users) {
    auto it = users.find(client_socket);
    if (it != users.end() && !it->second.get_nickname().empty()) { // l'erreur vient d'ici
        // Création d'un nouvel utilisateur si non trouvé
        User newUser;
        newUser.set_nickname(newNick);
        users[client_socket] = newUser;
    } else {
        // Mise à jour du pseudo pour un utilisateur existant
        string oldNick = it->second.get_nickname();
        it->second.set_nickname(newNick);

        // Format correct :nick!user@host NICK newnick
        string nickChangeMsg = ":" + oldNick + "!" + it->second.get_username() + "@" + it->second.get_hostname() + " NICK :" + newNick + "\r\n";
        send(client_socket, nickChangeMsg.c_str(), nickChangeMsg.length(), 0);

        // Envoyer ce message à tous les utilisateurs concernés, pas seulement à l'utilisateur qui change de pseudo
        // Cette étape dépend de la logique de votre application pour déterminer qui doit être informé.
    }
}
void Server::handle_user(int client_socket, const vector<string>& tokens, unordered_map<int, User>& users) {
    if (tokens.size() < 5) {
        string errMsg = ":localhost 461 * USER :Not enough parameters\r\n";
        send(client_socket, errMsg.c_str(), errMsg.length(), 0);
        return;
    }
    auto it = users.find(client_socket);
    if (it != users.end()) {
        it->second.set_username(tokens[1]);
        it->second.set_hostname(tokens[2]);
        it->second.set_servername(tokens[3]);
        it->second.set_realname(tokens[4]);
        it->second.userReceived = true;

        // Envoyez le message de bienvenue ou de confirmation de l'enregistrement ici
        cout << "coucou" << endl;
        if (it->second.passReceived) {
            string welcomeMsg = ":localhost 001 : " + it->second.get_nickname() + "\r\n";
            send(client_socket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
        }
        else {
            string ErrorMsgUser = ":localhost Registration failed, give the correct password, if you wan´t to register \r\n";
            send(client_socket, ErrorMsgUser.c_str(), ErrorMsgUser.length(), 0);
            close(client_socket);
        }
    }
}

void Server::handle_ping(int client_socket, const string& input) {
    string pongResponse = "PONG " + input + "\r\n";
    send(client_socket, pongResponse.c_str(), pongResponse.length(), 0);
}

void Server::handle_quit(int client_socket, unordered_map<int, User>& users) {
    string goodbyeMsg = ":localhost QUIT :Client has quit\r\n";
    send(client_socket, goodbyeMsg.c_str(), goodbyeMsg.length(), 0);

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
 
void Server::registration_done(int client_socket, const string& nick) {
    string regDoneMsg = ":localhost 001 " + nick + " Registration done!\r\n";
    if (send(client_socket, regDoneMsg.c_str(), regDoneMsg.size(), 0) <= 0) {
        cerr << "Error sending registration done message" << endl;
    } else {
        cout << GREEN << "IRCSERV" << RESET_COLOR << ": Registration done for " << nick << endl;
    }
}


void Server::setNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		throw runtime_error("Can't get flags for socket.");
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		throw runtime_error("Can't set socket to non-blocking.");
	}
}
