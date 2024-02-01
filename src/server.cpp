#include "Server.hpp"

using namespace std;

#define RESET_COLOR "\033[0m"

#define BLACK "\033[30m"         /* Black */
#define RED "\033[31m"           /* Red */
#define GREEN "\033[32m"         /* Green */
#define YELLOW "\033[33m"        /* Yellow */
#define BLUE "\033[34m"          /* Blue */
#define MAGENTA "\033[35m"       /* Magenta */
#define CYAN "\033[36m"          /* Cyan */
#define WHITE "\033[37m"         /* White */
#define BOLD_BLACK "\033[1m\033[30m"      /* Bold Black */
#define BOLD_RED "\033[1m\033[31m"        /* Bold Red */
#define BOLD_GREEN "\033[1m\033[32m"      /* Bold Green */
#define BOLD_YELLOW "\033[1m\033[33m"     /* Bold Yellow */
#define BOLD_BLUE "\033[1m\033[34m"       /* Bold Blue */
#define BOLD_MAGENTA "\033[1m\033[35m"    /* Bold Magenta */
#define BOLD_CYAN "\033[1m\033[36m"       /* Bold Cyan */
#define BOLD_WHITE "\033[1m\033[37m"      /* Bold White */

Server::Server(int port, string password) : _port(port), _password(password)
{
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": Port = " << port << "." << endl;
    cout << YELLOW << "IRCSERV" << RESET_COLOR << ": Password = ";
    for (int i = 0; i < size(password); i++) cout << "*";
    cout << "." << endl;
}

Server::~Server()
{

}

void    Server::Cold_Start(void)
{
    cout << BOLD_GREEN << "IRCSERV" << RESET_COLOR << ": Server cold start..." << endl; // Cold start :p
    struct sockaddr_in6 addr;
    struct vector<pollfd> poll_struct;
    string                all_data;

    this->_listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (this->_listen_fd < 0) throw runtime_error("Can't open the socket"); // Si _listen_fd est < 0 cela veut dire que socket fait mal son taff ce fou.
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket open..." << endl;
    
	this->setNonBlocking(this->_listen_fd);
    
    int opt = 1;
    if (setsockopt(this->_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) throw std::runtime_error("Can't set option socket.");
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket options set..." << endl;

	const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
	addr.sin6_family = AF_INET6;
	addr.sin6_scope_id = 0;
	addr.sin6_flowinfo = 0;
	addr.sin6_addr = in6addr_any;
	addr.sin6_port = htons(this->_port);
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": IPV6 addr set..." << endl;

    if (bind(this->_listen_fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind failed");
        close(this->_listen_fd);
        throw std::runtime_error("Can't bind socket.");
    }
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Socket bind success..." << endl;
    
    if ((listen(_listen_fd, 256)) < 0) throw std::runtime_error("Can't listen socket.");
    cout << GREEN << "IRCSERV" << RESET_COLOR << ": Listen socket success..." << endl;

	std::cout << BOLD_CYAN << "Wait connection..." << RESET_COLOR << std::endl;
	while (1)
        ;
}

void Server::setNonBlocking(int sock)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		throw std::runtime_error("Can't get flags for socket.");
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		throw std::runtime_error("Can't set socket to non-blocking.");
	}
}
