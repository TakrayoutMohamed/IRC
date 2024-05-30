#include "Commands.hpp"

void    start(int fd, std::string &str, std::map<int, Client> &client_map, std::vector<Channels> channels)
{
    if (IS_COMMAND_VALID(fd, str, client_map, channels))
        return ;
    // RUN_COMMAND(cmd);
    std::cout << "OK" << std::endl;
}

int main()
{
    int fd = 3;
    std::vector<Channels> channels;
    std::map<int, Client> client_map;
    Client client;
    client.nickname = "mohammed";
    client_map[3] = client;
    std::string input;
    std::cout << "Testing Start !!!" << std::endl;
    while (1)
    {
        std::cout << "client: ";
        std::getline(std::cin, input);
        if (input == "stop")
            break ;
        else
            start(fd, input, client_map, channels);
    }
    return (0);
}
