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
#include <sstream>
#include <iterator>

#include "user.hpp"
#include "channel.hpp"

#define BUFFER_SIZE 1024  // Ajustez la taille selon vos besoins

class Server {
    public:
        Server(char **av);
        Server(const Server& other);
        ~Server(void);

        Server& operator=(const Server& other);

        // ... vos autres méthodes ...

        void listen();
        void setNonBlocking(int sock);
        void _constructFds(); // Correction du nom de la fonction ici
        int getFD() const;
        void handleNewUser(int socketFD, const sockaddr_in6& clientAddr);
        void acceptNewConnections();
        void handleCompleteMessages(User* user, const std::string& data);
        void processClientInput(User* user);
        void removeUser(int userFD);
        std::vector<std::string> splitCommands(const std::string& data, char delimiter);
        void processCommand(const std::string& cmd, User* user);
        void sendMessage(int clientFD, const std::string& message);
        User* getUserByNickName(const std::string& nickName);
        Channel* getChannelByName(const std::string& channelName);

    private:
        std::string _nick_name;
        std::string _name;
        int _nb_user;
        std::string _password;
        int _port;
        std::vector<User *> _user;
        // std::vector<Channel *> _channel;
        std::vector<struct pollfd> _client_fds; // Changement du type ici
        int _server_fd;
        int _opt;
        int _server_socket;
        void _wait_connection(void);
        std::vector<Channel*> _channels;
};


#endif