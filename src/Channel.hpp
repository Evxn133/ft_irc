#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <unordered_set>
#include <string>

using namespace std;

class Channel {
    public:
        explicit Channel(const string& name);
    
        void addMember(int clientSocket);
        void removeMember(int clientSocket);
        bool isMember(int clientSocket) const;
        void broadcastMessage(const std::string& message, int senderSocket);
        const std::string& getName() const;
    
    private:
        string name;
        unordered_set<int> members;
};

#endif // CHANNEL_HPP
