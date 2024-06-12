#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <iostream>
# include <vector>
# include <map>

class Client
{
    private:
        /* data */
    public:
        std::string nickName;
        std::string userName;
        std::string realName;
        std::string hostName;
        std::string serverName;
        std::string ip;
        int fd;

        Client(/* args */);
        const Client &operator=(const Client &obj);
        ~Client();
};
#endif
