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

const std::string& Channel::getName() const {
    return name;
}
