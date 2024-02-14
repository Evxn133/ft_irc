#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include "Server.hpp"
#include "User.hpp"

using namespace std;

class Channel {
    public:
        explicit Channel(const string& name);
    
        void    addMember(int clientSocket);
        void    removeMember(int clientSocket);
        bool    isMember(int clientSocket) const;
        const   std::string& getName() const;
        // void    broadcastMessage(const std::string& message, int client_socket, const std::string& senderName, map<int, User>& users);
    
    private:
        string name;
        int     _fds;
};

#endif // CHANNEL_HPP
