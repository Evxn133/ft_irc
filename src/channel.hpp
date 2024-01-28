#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include "user.hpp"  // Assurez-vous que User est défini

class Channel {
public:
    Channel(const std::string& name) : _name(name) {}

    void addMember(User* user) {
        _members.push_back(user);
    }

    const std::vector<User*>& getMembers() const {
        return _members;
    }

    std::string getName() const {
        return _name;
    }

    // Ajoutez d'autres méthodes si nécessaire

private:
    std::string _name;
    std::vector<User*> _members;
};

#endif // CHANNEL_HPP
