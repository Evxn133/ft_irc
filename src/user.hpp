#ifndef USER_HPP
# define USER_HPP

#include <iostream>

class User {
	public:
		User(void);
		User(std::string name);
		User(const User& other);
		~User(void);

		User& operator=(const User& other);

        std::string getNickName();
        int getId();
		int getFD() const;
	private:
        std::string _nick_name;
        int _id;
		int	_fd;
};

#endif 