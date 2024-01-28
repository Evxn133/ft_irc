#ifndef USER_HPP
# define USER_HPP

#include <iostream>

class User {
	public:
    	void appendToRecvBuffer(const std::string& data) {
    	    _recvBuffer += data;
    	}

    	std::string getRecvBuffer() const {
    	    return _recvBuffer;
    	}

    	void clearRecvBuffer() {
    	    _recvBuffer.clear();
    	}
		User(void);
		User(std::string name);
		User(const User& other);
		~User(void);
		User(int fd);

		User& operator=(const User& other);

        std::string getNickName();
        int getId();
		int getFD() const;
        void setNickName(const std::string& nickName);

	private:
        std::string _nick_name;
        int _id;
		int	_fd;
		std::string _recvBuffer;
};

#endif 