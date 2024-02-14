#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <cstdlib>
#include <map>
#include <memory>
#include <stdio.h>

#include "User.hpp"
#include "Channel.hpp"

using namespace std;

class User;
class Channel;

class Server {

    public:

        Server(int port, string password);
        ~Server();

        // std::map<std::string, std::unique_ptr<Channel>> channels;
        void    Cold_Start(void);
        void    setNonBlocking(int sock);
        void    Welcome_Message(int client_socket, const string& nick);
        void    handle_input(int client_socket, map<int, User>& users, map<int, Channel>& chanels);
        void    command_handler(const string& command, int client_socket, const vector<string>& tokens, map<int, User>& users, map<int, Channel> channels);
        void    command_not_found(int client_socket, string command);
        void    registration_done(int client_socket, const string& nick);
        void    handle_nick(int client_socket, const string& input, map<int, User>& users);
        void    handle_user(int client_socket, const vector<string>& tokens, map<int, User>& users);
        void    handle_ping(int client_socket, const string& input);
        void    handle_cap_end(int client_socket, map<int, User>& users);
        void    handle_cap_ls(int client_socket, map<int, User>& users);
        void    handle_cap_req(int client_socket, const vector<string>& tokens, map<int, User>& users);
        void    handle_pass(int client_socket, const vector<string>& tokens, map<int, User>& users);
        void    handle_quit(int client_socket, map<int, User>& users);
        void    handle_whois(int client_socket, const vector<string>& tokens, map<int, User>& users);
        void    handle_join(int client_socket, const string& input, map<int, User>& users, map<int, Channel>& chanels);
        void    sendMOTD(int client_socket, map<int, User>& users);
        void    sendNotice(int client_socket, const std::string& message, const string& Hostname);
        void    sendMOTDLine(int client_socket, const std::string& message, const string& Nickname,  const string& Hostname);
        void    handle_privmsg(int client_socket, const vector<string>& tokens, map<int, User>& users);
        bool    nickReceived;
        bool    userReceived;
        bool    passReceived;
        // void    handle_unknown_command(int client_socket, const string& command);
    private:
        vector<User> _users;
        int     _listen_fd;
        int     _port;
        int     _nb_clients;
        bool    _bool_cmd;
        string  _password;
        bool    _password_match;
        bool    _minimal_reg;
        /*map<string, Channel> channels;*/
};

#endif
