#include "User.hpp"

using namespace std;

User::User(void) {
    this->_nick = "default";
    this->_user = "default";
}

User::~User(void) {

}

void    User::set_nickname(string nick_received) {
    this->_nick = nick_received;
}

void    User::set_client_fd(int fd_received) {
    this->_client_fd = fd_received;
}

string    User::get_nickname(void) {
    return (this->_nick);
}

int       User::get_client_fd(void) {
    return (this->_client_fd);
}

ostream& operator<<(std::ostream& os, const User& user) {
    os << "User: " << user._nick; // Afficher le surnom de l'utilisateur ou toute autre information pertinente
    return os;
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