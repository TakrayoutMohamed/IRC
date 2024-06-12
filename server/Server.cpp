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
	try
	{
		int	readyFds;
		Server serv(password, port);
		std::cout << "_password : " << serv.getPassword() << " _port = " << serv.getPort() << std::endl;
		serv.openSocket();
		serv.bindSocket();
		serv.listenSocket();
		while (1)
		{
			readyFds = serv.checkFdsForNewEvents();
			if (serv._socketsFds[0].revents & POLLIN)
			{
				serv.acceptClientSocket();
				serv.saveClientData();
				readyFds--;
				if (readyFds <= 0)
					continue ;
			}
 			for (size_t i = 1; i < serv._socketsFds.size(); i++)
			{
				if (serv._socketsFds[i].revents & (POLLRDNORM | POLLERR))
				{
					char msg[1024];
					bzero(msg, 1024);
					ssize_t recievedLen;
					recievedLen = recv(serv._socketsFds[i].fd, msg, 1024, 0);
					if (recievedLen > 512)
					{
						// send a responce to the client that sended the msg that says that the msg is too long
						const char *str = "line too long boy!!! \r\n";
						if (send(serv._socketsFds[i].fd, &str, sizeof(str), 0) == -1)
						{
							std::cerr << "There is an error with the send system call." << std::endl;
							exit(22);
						}
					}
					else if (recievedLen <= 0)
					{
						//here client closed connection than you should remove it since its not exist any more
						std::cerr << "the client with fd " << serv._socketsFds[i].fd << " has closed the connection"  << std::endl;
						serv._socketsFds.erase(serv._socketsFds.begin() + i);
						close(serv._socketsFds[i].fd);
						// here i need to use the quit command so that the users 
						//notified about the close of connection for this client
					}
					else
					{
						std::cout << "here the recieved message is good " << std::endl;
						//here the message is good so i have to pass this msg line to the command part
					}
					//here there is an event trigered by a client
					readyFds--;
				}
			}
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << std::endl;
		std::cerr << e.what() << std::endl;
	};
}

void Server::sendMsg(const std::string &msg, int fd) const
{
	std::string message;
	message = msg + "\r\n";
	send(fd, message.c_str(), message.length(), 0);
}

void Server::addData(int fd, const Client &client)
{
	this->_data[fd] = client;
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

const int &Server::getClientFd(void) const
{
   return (this->_clientFd);
}

const std::map<int, Client> &Server::getData(void) const
{
    return (this->_data);
}

void Server::openSocket(void)
{
	int	option;

	option = 1;
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw OpenServerSocketException();
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw NonBlockServerSocketException();
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		throw SocketCouldNotReuseAddrException();
}

int Server::bindSocket(void)
{
	_serverLen = sizeof(_serverAddr);
	bzero(&_serverAddr, _serverLen);
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_serverSocket, (sockaddr *) &_serverAddr, _serverLen) == -1)
		throw CouldNotBindServerSocketException();
    return 0;
}

int Server::listenSocket(void)
{
	pollfd tmp;

	tmp.fd = _serverSocket;
	tmp.events = POLLIN | POLLRDNORM;
	tmp.revents = 0;
	if (listen(_serverSocket, OPEN_MAX) == -1)
		throw CouldNotListenServerSocketException();
	_socketsFds.push_back(tmp);
    return 0;
}

int Server::checkFdsForNewEvents(void)
{
	int readyFds;

	readyFds = poll(_socketsFds.begin().base(), _socketsFds.size(), 0);
	if (readyFds == -1)
		throw PollCheckFdsEventsException();
    return (readyFds);
}

int Server::acceptClientSocket(void)
{
	_clientLen = sizeof(_clientAddr);
	bzero(&_clientAddr, _clientLen);
	_clientFd = accept(_serverSocket, (sockaddr *) &_clientAddr, &_clientLen);
	if (_clientFd == -1)
		throw NewClientNotAcceptedException();
	if (_socketsFds.size() >= OPEN_MAX)
		std::cerr << "Error Too many Clients "<< std::endl;
    return 0;
}

int Server::saveClientData(void)
{
	Client client;
	pollfd tmp;
	char hostname[_SC_HOST_NAME_MAX + 1];

	tmp.fd = _clientFd;
	tmp.events = POLLIN | POLLRDNORM;
	tmp.revents = 0;
	//here check the authentication
	if (Authenticator::checkClientAuthentication(*this, client))
	{
		if (gethostname(hostname, _SC_HOST_NAME_MAX))
		{
			sendMsg("Quit : error while trying to get the hostname", _clientFd);
			close(_clientFd);
			return (1);
		}
		client.fd = _clientFd;
		client.hostName = hostname;
		client.ip = inet_ntoa(_clientAddr.sin_addr);
		addData(_clientFd, client);
		_socketsFds.push_back(tmp);
		_clientFd = -2;
	}
    return 0;
}

int Server::deleteClientFd(int fd)
{
	(void) fd;
    return 0;
}

int Server::readClientFd(int fd)
{
	(void) fd;
    return 0;
}


/********************************Exceptions*********************************/

const char *Server::PortNotAcceptedException::what() const throw()
{
	return "Port not accepted , the port has to be an integer between 1024 and 65535.";
}
