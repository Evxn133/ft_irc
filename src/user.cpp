#include "user.hpp"

// constructor

User::User(void) {
    _nick_name = "name";
    _id = -1;
    _fd = -1;
}

User::User(std::string name) {
    _nick_name = name;
    _id = 12;
    _fd = -1;
}

User::User(const User& other) {

}

// destructor

User::~User(void) {

}

// operator

User &User::operator=(const User& other) {
    _nick_name = other._nick_name;
    _id = other._id;
	return *this;
}

// other

std::string User::getNickName() {
    return _nick_name;
}


int User::getId() {
    return _id;
}

int User::getFD() const {
    return this->_fd;  // Utilisez le nom réel de votre variable membre
}
