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
					Client &triggeredClient = serv.getData().find(serv._socketsFds[i].fd)->second;
					recievedLen = recv(serv._socketsFds[i].fd, msg, 1024, 0);
					std::string line(msg);
					if (!serv.handleCtrlD(line, triggeredClient.bufferString))
					{
						readyFds--;
						continue ;
					}
					if (recievedLen > 512)
					{
						// send a response to the client that sended the msg that says that the msg is too long
						serv.sendMsg("line too long boy!!!", triggeredClient.fd);
					}
					else if (recievedLen <= 0)
					{
						//here client closed connection than you should remove it since its not exist any more
						std::cerr << "the client with fd " << serv._socketsFds[i].fd << " has closed the connection"  << std::endl;
						close(serv._socketsFds[i].fd);
						serv.getData().erase(triggeredClient.fd);
						serv._socketsFds.erase(serv._socketsFds.begin() + i, serv._socketsFds.begin() + i + 1);
						// here i need to use the quit command so that the users 
						//notified about the close of connection for this client
					}
					else
					{
						if (triggeredClient.isAuthenticated == false)
							Authenticator::checkClientAuthentication(serv, triggeredClient, line);
						else
						{
							std::cout << "the client with fd = {"<< triggeredClient.fd <<"} is authenticated" << std::endl;
							std::cout << "nickName = {"<< triggeredClient.nickName <<"}" << std::endl;
							std::cout << "userName = {"<< triggeredClient.userName <<"}" << std::endl;
							std::cout << "realName = {"<< triggeredClient.realName <<"}" << std::endl;
							std::cout << "hostName = {"<< triggeredClient.hostName <<"}" << std::endl;
							std::cout << "serverName = {"<< triggeredClient.serverName <<"}" << std::endl;
							std::cout << "here the recieved message is:{" << line <<"}" << std::endl;
							//here the message is good so i have to pass this msg line to the command part
						}
					}
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
