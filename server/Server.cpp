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
		signal(SIGPIPE, SIG_IGN);
		while (1)
		{
			readyFds = serv.checkFdsForNewEvents();
			if (readyFds == 0)
				continue ;
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
				if (!(serv._socketsFds[i].revents & (POLLRDNORM | POLLERR)))
					continue ;
				serv.clientWithEvent(readyFds, i);
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
	std::pair<int, Client> data;
	data.first = fd;
	data.second = client;
	this->_data.insert(data);
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

std::map<int, Client> &Server::getData(void)
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
	if (gethostname(hostname, _SC_HOST_NAME_MAX))
	{
		sendMsg("Quit : error while trying to get the hostname", _clientFd);
		/*here i need to apply the quit command*/
		// _socketsFds.erase(std::find(_socketsFds.begin(), _socketsFds.end(), _clientFd));
		close(_clientFd);
	}
	client.fd = _clientFd;
	client.hostName = hostname;
	client.ip = inet_ntoa(_clientAddr.sin_addr);
	addData(_clientFd, client);
	_socketsFds.push_back(tmp);
	
    return 0;
}

void Server::clientCloseConnextion(const int clientIndex)
{
	//here client closed connection than you should remove it since its not exist any more
	// here i need to use the quit command so that the users 
	//notified about the close of connection for this client
	std::cerr << "the client with fd " << this->_socketsFds[clientIndex].fd << " has closed the connection"  << std::endl;
	close(this->_socketsFds[clientIndex].fd);
	this->getData().erase(_socketsFds[clientIndex].fd);
	this->_socketsFds.erase(this->_socketsFds.begin() + clientIndex, this->_socketsFds.begin() + clientIndex + 1);
}

void Server::clientWithEvent(int &readyFds,const int clientIndex)
{
	char		msg[1024];
	int			countNewLine;
	std::string line;
	Client		&triggeredClient = this->getData().find(this->_socketsFds[clientIndex].fd)->second;

	bzero(msg, 1024);
	recv(this->_socketsFds[clientIndex].fd, msg, 1024, 0);
	line = msg;
	std::cout << "recieved line is {"<< line << "}"<< std::endl;
	if (!this->handleCtrlD(line, triggeredClient.bufferString))
		return ;
	if (line.length() > 512)
	{
		this->sendMsg(triggeredClient.ip + ": ERR_INPUTTOOLONG (417):Input line was too long", triggeredClient.fd);
		return ;
	}
	else if (line.length() <= 0)
	{
		this->clientCloseConnextion(clientIndex);
		return ;
	}
	countNewLine = countNewLines(line);
	std::cout << "count New Lines = " << countNewLine << std::endl;
	this->pushLineToStream(line);
	while (countNewLine-- >= 1)
	{
		line.clear();
		std::getline(_stringStream, line, '\n');
		if (line.find('\r') != line.npos)
			line.erase(line.find('\r'));
		std::cout << "line after spliting it is :{" << line << "}" << std::endl;
		if (triggeredClient.isAuthenticated == false)
			Authenticator::checkClientAuthentication(*this, triggeredClient, line);
		else
		{
			this->sendMsg(triggeredClient.ip + "you have been authenticated successfully", triggeredClient.fd);
			//here YOU ARE GOING TO PASS THE LINE TO THE COMMANDS PART 
		}
	}
}

std::stringstream	&Server::pushLineToStream(const std::string &line)
{
	this->_stringStream.clear();
	this->_stringStream.str("");
	this->_stringStream.str(line);
	return (this->_stringStream);
}

int	countNewLines(const std::string &line)
{
	int	nbr;

	nbr = 0;
	for (size_t i = 0; i < line.length(); i++)
	{
		if (line[i] == '\n')
			nbr++;
	}
	return (nbr);
}

bool Server::handleCtrlD(std::string &line, std::string &bufferString)
{
	if (line.length() == 0)
		return (true);
	if (line.find('\n') == std::string::npos)
	{
		bufferString.append(line);
		return (false);
	}
	if(bufferString.length() > 0)
	{
		bufferString.append(line);
		line.clear();
		line = "";
		line.append(bufferString);
		bufferString.clear();
		bufferString = "";
	}
	return (true);
}

/********************************Exceptions*********************************/

const char *Server::PortNotAcceptedException::what() const throw()
{
	return "Port not accepted , the port has to be an integer between 1024 and 65535.";
}
