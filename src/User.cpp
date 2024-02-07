#include "User.hpp"

using namespace std;

User::User(void) {
}

User::~User(void) {

}

void    User::set_nickname(string nick_received) {
    this->_nick = nick_received;
}

void    User::set_client_fd(int fd_received) {
    this->_client_fd = fd_received;
}

void    User::set_username(string username_received) {
    this->_username = username_received;
}

void    User::set_hostname(string hostname_received) {
    this->_hostname = hostname_received;
}

void    User::set_realname(string realname_received) {
    this->_realname = realname_received;
}

void    User::set_servername(string servername_received) {
    this->_server_name = servername_received;
}

string    User::get_nickname(void) const {
    return (this->_nick);
}

int       User::get_client_fd(void) const {
    return (this->_client_fd);
}

string    User::get_username(void) const {
    return (this->_username);
}
string    User::get_hostname(void) const {
    return (this->_hostname);
}

string    User::get_realname(void) const {
    return (this->_realname);
}

string    User::get_servername(void) const {
    return (this->_server_name);
}

ostream& operator<<(ostream& os, const User& user) {
    os << "User: " << user._nick; // Afficher le surnom de l'utilisateur ou toute autre information pertinente
    return os;
}
