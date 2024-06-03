#include "./Server.hpp"
#include "Server.hpp"

Server::Server() : _port(0), _password("   ")
{

}

Server::Server(const Server &obj)
{
	if (this != &obj)
	{
		*this = obj;
	}
}

Server::Server(const std::string &password, const int port)
{
	if (!isValidPassword(password))
		throw PasswordNotAcceptedException;
	if (!isValidPort(port))
		throw PortNotAcceptedException;
	setPassword(password);
	setPort(port);

}

const Server &Server::operator=(const Server &obj)
{
	if (this != &obj)
	{
		this->_port = obj._port;
		this->_password = obj._password;
	}
	return (*this);
}

Server::~Server()
{

}

bool	hasSpace(const std::string &str)
{
	int i;

	i = 0;
	while (i < str.length())
	{
		if (std::isspace(static_cast<unsigned char>(str[i])))
			return (true);
		i++;
	}
	return (false);
}

const bool	Server::isValidPassword(const std::string &password) const
{
	if (password.length() < 4)
		return (false); /*password has to be more than 4 char in length */
	if (!hasSpace(password))
		return (false); /*password does not accept whitespaces*/
	return (true);
}

const bool Server::isValidPort(const int port) const
{
	return false;
}

void	Server::setPassword(const std::string &password)
{
	this->_password = password;
}

void	Server::setPort(const int &port)
{
	this->_port = port;
}
