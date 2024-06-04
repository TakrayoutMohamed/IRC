#include "Server.hpp"

Server::Server() : _password("DefaultPassword"), _port(0) 
{

}

Server::Server(const Server &obj)
{
	if (this != &obj)
	{
		*this = obj;
	}
}

Server::Server(const std::string &password, const std::string &port) : _password("DefaultPassword"), _port(0)
{
	int	port_nbr;

	if (!isValidPassword(password))
		throw PasswordNotAcceptedException();
	port_nbr = convertStringToInt(port);
	if (!isValidPort(port_nbr))
		throw PortNotAcceptedException;
	setPassword(password);
	setPort(port_nbr);

}

const Server &Server::operator=(const Server &obj)
{
	if (this != &obj)
	{
		this->_password = obj._password;
		this->_port = obj._port;
	}
	return (*this);
}

Server::~Server()
{

}

void Server::runServer(const std::string &password, const std::string &port)
{
	int	fd_socket;
	ssize_t n;
	socklen_t clientLen;
	struct pollfd       client[OPEN_MAX];
	struct sockaddr_in  clientaddr, serveraddr;
	try
	{
		Server serv(password, port);
		std::cout << "_password : " << serv.getPassword() << " _port = " << serv.getPort() << std::endl;
		
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << std::endl;
		std::cerr << e.what() << std::endl;
	};
}

bool	hasSpace(const std::string &str)
{
	size_t i;

	i = 0;
	while (i < str.length())
	{
		if (std::isspace(static_cast<unsigned char>(str[i])))
			return (true);
		i++;
	}
	return (false);
}

bool	Server::isValidPassword(const std::string &password) const
{
	if (password.length() < 4)
		return (false); /*password has to be more than 4 char in length */
	if (hasSpace(password))
		return (false); /*password does not accept whitespaces*/
	return (true);
}

int	convertStringToInt(const std::string &str)
{
	std::stringstream sstream(str);
	int nbr;

	sstream >> nbr;
	if (sstream.fail() || sstream.bad())
		return (-1);
	return (nbr);
}

bool Server::isValidPort(const int port) const
{
	if (port >= 1024 && port <= 65535)
		return (true); /*port has to be between 1024 and 65535*/
	return (false);
}

/******************************** setters **********************************/

void	Server::setPassword(const std::string &password)
{
	this->_password = password;
}

void	Server::setPort(const int &port)
{
	this->_port = port;
}

/******************************** getters **********************************/

const std::string &Server::getPassword(void) const
{
	return (this->_password);
}

const int &Server::getPort(void) const
{
	return (this->_port);
}

/********************************Exceptions*********************************/

const char *Server::PortNotAcceptedException::what() const throw()
{
	return "Port not accepted , the port has to be an integer between 1024 and 65535.";
}
