#include "Server.hpp"

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
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": Password = ";
    for (int i = 0; i < password.size(); i++) cout << "*";
    cout << "." << endl;
    _nb_clients = 0;
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

        std::unordered_map<int, User> users; // Map pour associer les sockets aux utilisateurs
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
                    Welcome_Message(new_socket); // Envoyer un message de bienvenue
                } else {
                    // Gérer l'entrée d'un client existant
                    handle_input(poll_struct[i].fd, users);
                }
            }
        }
    }
}

void Server::handle_input(int client_socket, std::unordered_map<int, User>& users) {
    char buf[1024];
    int bytesReceived = recv(client_socket, buf, 1024, 0);
    if (bytesReceived <= 0) {
        // ... (gestion de la déconnexion et des erreurs)
        return;
    }

    string ClientMsg(buf, bytesReceived);
    istringstream iss(ClientMsg);
    vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};

    if (!tokens.empty()) {
        const string& command = tokens[0];
        string input = tokens.size() > 1 ? tokens[1] : ""; // Assurez-vous qu'il y a un deuxième token avant d'y accéder

        if (command == "NICK") {
            auto it = users.find(client_socket);
            if (it != users.end()) {
                // Si l'utilisateur est trouvé, mettez à jour son surnom
                it->second.set_nickname(input);
            } else {
                // Sinon, créez un nouvel utilisateur
                User newUser;
                newUser.set_client_fd(client_socket);
                newUser.set_nickname(input);
                users[client_socket] = newUser;
            }
            cout << GREEN << "IRCSERV" << RESET_COLOR << ": The client with the fd " << client_socket << " has the nickname " << MAGENTA << users[client_socket].get_nickname() << RESET_COLOR << endl;
        }
        else if (command == "USER") {
            if (tokens.size() < 5) {
                // Pas assez de paramètres
                // Envoyer un message d'erreur au client
                std::string errMsg = "ERROR :USER command requires more parameters\r\n";
                send(client_socket, errMsg.c_str(), errMsg.length(), 0);
                return;
            }
            // Extraction des informations
            string username = tokens[1];
            string hostname = tokens[2];
            string servername = tokens[3];
            string realname = tokens[4]; // realname peut contenir des espaces, donc vous devrez peut-être le traiter différemment
            auto it = users.find(client_socket);
            if (it != users.end()) {
                // Si l'utilisateur est trouvé, mettez à jour ses informations
                it->second.set_username(username);
                it->second.set_hostname(hostname);
                it->second.set_servername(servername);
                it->second.set_realname(realname);
            } else {
                // Sinon, créez un nouvel utilisateur (ou gérez l'erreur si l'utilisateur doit déjà exister à ce stade)
                User newUser;
                newUser.set_client_fd(client_socket);
                newUser.set_username(username);
                newUser.set_hostname(hostname);
                newUser.set_servername(servername);
                newUser.set_realname(realname);
                users[client_socket] = newUser;
            }
        }
        else if (command == "PING") {
            string PongMsg = "PONG " + input + "\r\n";
            if (send(client_socket, PongMsg.c_str(), PongMsg.size(), 0) <= 0) {
                cerr << "Error message de bienvenue" << endl;
                return ;
            }
        }
        else if (command == "QUIT") {
            // Message d'adieu :(
            std::string farewellMsg = "Goodbye!\r\n";
            send(client_socket, farewellMsg.c_str(), farewellMsg.length(), 0);
            // Fermer le socket
            close(client_socket);
            // Supprimer l'utilisateur de la map
            users.erase(client_socket);
        
            // Afficher un message dans le serveur ou effectuer d'autres actions de nettoyage
            cout << "Le client " << client_socket << " s'est déconnecté avec QUIT." << endl;
        
            // Si vous avez un système de notification pour les autres utilisateurs ou des canaux, gérez-le ici
            // ...
        }
    }

}

// void    Server::command_not_found(int client_socket, string command) {
//     string notFoundMsg = "421 evan :The command  :" + command + " does not exist \r\n";
    
//     if (send(client_socket, notFoundMsg.c_str(), notFoundMsg.size(), 0) <= 0) {
//         cerr << "Error message de unknowncomamnd (421)" << endl;
//         return ;
//     }
// }
 
void    Server::Welcome_Message(int client_socket) {//, const string& nick, const string& username, const string& hostname, const string& network_name) a ajouter plus tard
    string WelcomeMsg = "001 evan :IRCSERV: Welcome to the IRCSERV Network Evan(nick)!\r\n";

    if (send(client_socket, WelcomeMsg.c_str(), WelcomeMsg.size(), 0) <= 0) {
        cerr << "Error message de bienvenue" << endl;
        return ;
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
