#include "./../server/Server.hpp"

int main(const int argc, const char **argv)
{
    (void )(argv);
    (void ) (argc);
    try
    {
        Server::runServer("aaaa", "4444");
    }
    catch(const std::exception& e)
    {
        std::cout << "**********here is the TestServer exception************" << std::endl;
        std::cerr << e.what() << '\n';
    }
    

    return (0);
}