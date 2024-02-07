#include "Channel.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string& name) : name(name) {}

void Channel::addMember(int clientSocket) {
    members.insert(clientSocket);
}

void Channel::removeMember(int clientSocket) {
    members.erase(clientSocket);
}

bool Channel::isMember(int clientSocket) const {
    return members.find(clientSocket) != members.end();
}

void Channel::broadcastMessage(const std::string& message, int senderSocket) {
    for (int memberSocket : members) {
        if (memberSocket != senderSocket) {
            send(memberSocket, message.c_str(), message.length(), 0);
        }
    }
}

const std::string& Channel::getName() const {
    return name;
}
