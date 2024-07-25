#include "./server/Server.hpp"
# include <iostream>
# include <unistd.h> /*close()*/
# include <sys/socket.h> /*socket() , connect()*/
# include <arpa/inet.h> /*htons*/
# include <netinet/in.h> /*sockaddr_in*/
int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Error:"<< std::endl;
        std::cerr << "The valid format is : (./bot <serverAddress> <port> <password>)." << std::endl;
        return (1);
	}
	std::string serverAddress = argv[1];
	struct sockaddr_in	botAddr;
	socklen_t			addrLen;
	int port = atoi(argv[2]);
	std::string password = argv[3];
	int botSocket;
	int client;
	char recieveMsg[1024];

	botSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (botSocket == -1)
	{
		std::cerr << "ERROR:" << std::endl;
		std::cerr << "Could not open the socket for the bot ." << std::endl;
		return (1);
	}

	addrLen = sizeof(botAddr);
	bzero(&botAddr, addrLen);
	botAddr.sin_family = AF_INET;
	botAddr.sin_port = htons(port);
	botAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
	client = connect(botSocket, (struct sockaddr *) &botAddr, addrLen);
	if (client == -1)
	{
		std::cerr << "ERROR:" << std::endl;
		std::cerr << "Could not Connect to the server." << std::endl;
		if (botAddr.sin_addr.s_addr == INADDR_NONE)
			std::cerr << "Address you entered for server not accepted!!" << std::endl;
		return (close(botSocket), 1);
	}
	if (fcntl(botSocket, F_SETFL, O_NONBLOCK) == -1)
		throw NonBlockServerSocketException();
	std::string str = "pass " + password + "\nnick bot\nuser bot bot bot bot\r\n";
	send(botSocket, str.c_str(), str.length(),0);
	char str3[111] = "usssss\r\n";
	send(botSocket, str3, strlen(str3), 0);
	std::cout << "the bot is running ..." << std::endl;
	int ret;
	while (1)
	{
		ret = recv(botSocket, recieveMsg, 1024, MSG_DONTWAIT);
		if (ret < 1  && (errno != EAGAIN && errno != EWOULDBLOCK))
		{
			std::cout << "return = "<< ret << "" << std::endl;
			std::cout << "errno = "<< errno << "" << std::endl;
			return (44);
		}
		else if (ret >= 1)
		{
			std::cout << recieveMsg << std::endl;

		}
		// if (ret == 0)
		// {
		// 	std::cout << "return = "<< ret << "" << std::endl;
		// 	std::cout << "errno = "<< errno << "" << std::endl;
		// }
		errno = 0;
	}
	return (0);
}