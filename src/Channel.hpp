#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <unordered_set>
#include <string>
#include "Server.hpp"
#include "User.hpp"

using namespace std;

class Channel {
    public:
        explicit Channel(const string& name);
    
        void addMember(int clientSocket);
        void removeMember(int clientSocket);
        bool isMember(int clientSocket) const;
        const std::string& getName() const;
        // void broadcastMessage(const std::string& message, const std::string& senderName, unordered_map<int, User>& users);
    
    private:
        string name;
        unordered_set<int> members;
};

#endif // CHANNEL_HPP
