#include "Server.hpp"

#define RESET_COLOR "\033[0m"

#define BLACK "\033[30m"                    /* Black */
#define RED "\033[31m"                      /* Red */
#define GREEN "\033[32m"                    /* Green */
#define YELLOW "\033[33m"                   /* Yellow */
#define BLUE "\033[34m"                     /* Blue */
#define MAGENTA "\033[35m"                  /* Magenta */
#define CYAN "\033[36m"                     /* Cyan */
#define WHITE "\033[37m"                    /* White */
#define BOLD_BLACK "\033[1m\033[30m"        /* Bold Black */
#define BOLD_RED "\033[1m\033[31m"          /* Bold Red */
#define BOLD_GREEN "\033[1m\033[32m"        /* Bold Green */
#define BOLD_YELLOW "\033[1m\033[33m"       /* Bold Yellow */
#define BOLD_BLUE "\033[1m\033[34m"         /* Bold Blue */
#define BOLD_MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLD_CYAN "\033[1m\033[36m"         /* Bold Cyan */
#define BOLD_WHITE "\033[1m\033[37m"        /* Bold White */

using namespace std;

void digit_in(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] < '0' || str[i] > '9')
            throw runtime_error("Usage: The port need only digits.");
    }
}

void password_check(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] < 33 || str[i] > 126)
            throw runtime_error("Usage: Password must contains only visible char.");
    }
}

void    parse_arg(int argc, char **argv)
{
    try
    {
        if (argc != 3) throw runtime_error("Usage: ./ircserv <port> <password>.");
        digit_in(argv[1]);
        password_check(argv[2]);
    }
    catch (const runtime_error& e)
    {
        cerr << RED << e.what() << RESET_COLOR << endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{
    parse_arg(argc, argv); // Je parse mes ptits trucs la (try catch le goat).

    Server server(atoi(argv[1]), argv[2]); // Assigne port et password dans le constructeur.
    try
    {
        server.Cold_Start();
    }
    catch (const runtime_error& e)
    {
        cerr << BOLD_RED << "IRC_SERV_ERROR: " << RED << e.what() << RESET_COLOR << '\n';
        exit(EXIT_FAILURE);
    }
    return 0;
}
