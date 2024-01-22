#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <algorithm>

#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

#include "server.hpp"
#include "user.hpp"

int main(int ac, char **av) {

    if (ac != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    Server server(av);

    User userTest1("test1");
    server.addUser(userTest1);

    server.printUser();

    while (true) {
        ;// server_running
    }
    
    // fin de processus, liberer les port etc
    return 0;
}


/*int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::string password = argv[2];
    int server_fd, new_socket;
    struct sockaddr_in address;
    std::vector<struct pollfd> fds;
    char buffer[BUFFER_SIZE];
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd < 0) {
        perror("Échec de la création du socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Échec de la liaison");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Écoute");
        exit(EXIT_FAILURE);
    }

    fds.push_back({ .fd = server_fd, .events = POLLIN });

	int	nb_connection = 0;
    while (true) {
        if (poll(fds.data(), fds.size(), -1) < 0) {
            perror("Erreur poll");
            break;
        }
        for (auto &pfd : fds) {
            if (pfd.revents & POLLIN) {
                if (pfd.fd == server_fd) {
					nb_connection++;
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    if (new_socket < 0) {
                        perror("Acceptation");
                        continue;
                    }
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    fds.push_back({ .fd = new_socket, .events = POLLIN });
					std::cout << "Nombre de client sur le server : " << nb_connection << std::endl;

					const char *welcome_message = "Bienvenue sur le serveur IRC!\n";
        			send(new_socket, welcome_message, strlen(welcome_message), 0);
                } else {
                    memset(buffer, 0, BUFFER_SIZE);
                    int count = read(pfd.fd, buffer, BUFFER_SIZE);
                    if (count > 0) {
                        std::cout << "Message reçu d'un client: " << buffer << std::endl;
						std::cout << buffer << std::endl;
                    } else {
                        close(pfd.fd);
						nb_connection--;
						std::cout << "Nombre de client sur le server : " << nb_connection << std::endl;
                        pfd.fd = -1;
                    }
                }
            }
        }

        fds.erase(std::remove_if(fds.begin(), fds.end(), [](const struct pollfd &pfd) { return pfd.fd == -1; }), fds.end());
    }

	std::cout << "Fermeture du serveur et déconnexion des clients..." << std::endl;
    for (auto &pfd : fds) {
        if (pfd.fd >= 0) {
            close(pfd.fd);
        }
    }

    return 0;
}*/



/*client :
id = 55590
name = evan

channel :
id = 111
name = lala
list log user = [55590, ]        1
list user admin = [55590, ]      2
list user createur = [55590, ]   3

[[1, ,.,.,.,.,..,.,., 12,,.,.,.,.,.,.,..,.,., 25,,.,.,.,.,..,,..,.,., 63] [0, 2, 1, 0]]

--------------------------------------

join_channel(channel velo, int  user)

channel(channel **name, int user)
int user_id = 
if (channel[user_id] > 0)
	ok il peut passer
if (channel[user_id] == 2)
	ok tu peux kick
if (channel[user_id] == 3)
	createur
else
	error*/

// 