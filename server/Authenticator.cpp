#include "./Authenticator.hpp"
#include "Authenticator.hpp"

Authenticator::Authenticator(/* args */) : _nick(""), _user(""), _realName("")
{

}

Authenticator::~Authenticator()
{
}

std::stringstream	&Authenticator::pushLineToStream(const std::string &line)
{
	this->_stringStream.clear();
	this->_stringStream.str("");
	this->_stringStream.str(line);
	return (this->_stringStream);
}

void Authenticator::toUpper(std::string &str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
	{
		if (islower(static_cast<int> (str[i])))
			str[i] = toupper(static_cast<int> (str[i]));
		i++;
	}
}

void	Authenticator::parsePass(std::string &cmd, std::string &password, std::string &third)
{
	std::getline(this->_stringStream, cmd, ' ');
	skipSpaces(this->_stringStream, password);
	skipSpaces(this->_stringStream, third);
}

int Authenticator::checkPassword(const Server &server, int fd)
{
	std::string pass;
	std::string password;
	std::string thirdParam;

	parsePass(pass, password, thirdParam);
	if (thirdParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS ()", fd);
		return (false);
	}
	this->toUpper(pass);
	if (password.length() == 0)
	{
		server.sendReply("461", "not enough parameters", this->_client);

		return (false);
	}
	if (server.getPassword().compare(password) == 0)
		return (true);
	server.sendReply("464", "password incorect", this->_client);
	return (false);
}

bool hasUnacceptedChars(const std::string &nick)
{
	for (size_t i = 0; i < nick.size(); i++)
	{
		if (isspace(static_cast<int>(nick[i])))
			return (true);
		if (nick[i] == ':')
			return (true);
		if (nick[i] == '#')
			return (true);
		if (nick[i] == '@')
			return (true);
		if (nick[i] == '&')
			return (true);
	}
	return (false);
}

bool	isNickNameInUse(const std::map<int, Client> &data, const std::string &nick)
{
	std::map<int, Client>::const_iterator  it = data.begin();
	while (it != data.end())
	{
		if (it->second.nickName.compare(nick) == 0)
			return (true);
		it++;
	}
	return (false);
}

/// @brief skip the spaces and asign the text from a ' ' space to another into str parameter
/// @param sstream  a stream of characters 
/// @param str a string to asign the first occurs of non space characters
void skipSpaces(std::stringstream &sstream, std::string &str)
{
	std::getline(sstream, str, ' ');
	while(str.compare("") == 0 && !sstream.fail())
	{
		std::getline(sstream, str, ' ');
	}
}

void	Authenticator::parseNick(std::string &cmd, std::string &nickName, std::string &third)
{
	std::getline(this->_stringStream, cmd, ' ');
	skipSpaces(this->_stringStream, nickName);
	skipSpaces(this->_stringStream, third);
}

int Authenticator::checkNick(Server &server, int fd)
{
	std::string firstParam;
	std::string secondParam;
	std::string thirdParam;

	parseNick(firstParam, secondParam, thirdParam);
	if (thirdParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS ()", fd);
		return (false);
	}
	if (hasUnacceptedChars(secondParam))
	{
		server.sendMsg(secondParam + " : Erroneus nickname", fd);
		return (false);
	}
	if (secondParam.length() == 0)
	{
		server.sendMsg(": no nickname is given", fd);
		return (false);
	}
	if (isNickNameInUse(server.getData(), secondParam))
	{
		server.sendMsg(secondParam + " : nickname already in use", fd);
		return (false);
	}
	this->setNick(secondParam);
	return (true);
}

void	Authenticator::parseUser(std::string &cmd, std::string &user, std::string &hostName, std::string &servName, std::string &realName, std::string &sixthParam)
{
	std::getline(this->_stringStream, cmd, ':');
	std::getline(this->_stringStream, realName, ':');
	std::getline(this->_stringStream, sixthParam, ':');
	this->pushLineToStream(cmd);
	cmd.clear();
	cmd = "";
	std::getline(this->_stringStream, cmd, ' ');
	skipSpaces(this->_stringStream, user);
	skipSpaces(this->_stringStream, hostName);
	skipSpaces(this->_stringStream, servName);
	if (realName.length() == 0)
	{
		skipSpaces(this->_stringStream, realName);
		skipSpaces(this->_stringStream, sixthParam);
	}
}

int Authenticator::checkUser(const Server &server, int fd)
{
	std::string firstParam;
	std::string secondParam;
	std::string thirdParam;
	std::string fourthParam;
	std::string fifthParam;
	std::string sixthParam;

	parseUser(firstParam, secondParam, thirdParam, fourthParam, fifthParam, sixthParam);
	if (sixthParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS IN USER()", fd);
		return (false);
	}
	if (fifthParam.length() == 0)
	{
		this->toUpper(firstParam);
		server.sendMsg(firstParam + " : not enough parameters", fd);
		return (false);
	}
	this->setUser(("~" + secondParam).c_str());
	this->setRealName(fifthParam);
    return (true);
}

void printClientData(const Client &client)
{
	std::cout << "*************************************" << std::endl;
	std::cout << "isAuthenticated = {"<< client.isAuthenticated <<"}" << std::endl;
	std::cout << "nickName = {"<< client.nickName <<"}" << std::endl;
	std::cout << "userName = {"<< client.userName <<"}" << std::endl;
	std::cout << "realName = {"<< client.realName <<"}" << std::endl;
	std::cout << "hostName = {"<< client.hostName <<"}" << std::endl;
	std::cout << "serverName = {"<< client.serverName <<"}" << std::endl;
	std::cout << "ip = {"<< client.ip <<"}" << std::endl;
	std::cout << "fd = {"<< client.fd <<"}" << std::endl;
	std::cout << "*************************************" << std::endl;
}

void Authenticator::welcomeMsg(const Server &server, const Client &client)
{
	std::string rplWelcome;
	std::string rplYourhost;
	std::string rplCreated;
	std::string rplMyInfo;

	rplWelcome = client.ip + " : Welcome to FT_IRC  Network, ";
	rplWelcome += client.nickName +"!"+ client.userName + "@" + client.ip;
	server.sendMsg(rplWelcome, client.fd);
	rplYourhost = client.ip + " : your host is " + client.serverName + ", runing version 1.1 ";
	server.sendMsg(rplYourhost, client.fd);
	rplCreated = client.ip + " : this server was created " + getDateTime(server.getCreateDate());
	server.sendMsg(rplCreated, client.fd);
	rplMyInfo = ":Host: " + client.ip + ", Version: 1.1, User mode: none, Channel modes: o, t, k, i, l !";
	server.sendMsg(rplMyInfo, client.fd);
}

int Authenticator::checkClientAuthentication(Server &server, Client &client, std::string &line)
{
	Authenticator auth;

	auth._client = client;
	if (line.find('\n') != line.npos)
		line.erase(line.find('\n'));
	auth.pushLineToStream(line);
	for (int i = 0; i < 4 && line[i] != '\0'; i++)
		line[i] = toupper(static_cast<int>(line[i]));
	if (line.compare(0, 5, "PASS ", 0, 5) != 0 && !client._isPassSet)
	{
		server.sendMsg("YOU NEED TO SET THE SERVER'S PASSWORD FIRST", client.fd);
		return (false);
	}
	if (line.compare(0, 5, "PASS ", 0, 5) == 0)
		client._isPassSet = auth.checkPassword(server, client.fd);
	else if (line.compare(0, 5, "USER ", 0, 5) == 0)
	{
		if (!client._isUserSet && auth.checkUser(server, client.fd))
		{
			client._isUserSet = true;
			client.userName = auth._user;
			client.realName = auth._realName;
		}
	}
	else if (line.compare(0, 5, "NICK ", 0, 5) == 0)
	{
		if (!client._isNickSet && auth.checkNick(server, client.fd))
		{
			client._isNickSet = true;
			client.nickName = auth._nick;
		}
	}
	else if (line.length() > 0)
	{
		server.sendMsg(": you have not registered", client.fd);
		printClientData(client);
	}
	if (client._isNickSet && client._isPassSet && client._isUserSet)
	{
		client.isAuthenticated = true;
		auth.welcomeMsg(server, client);
	}
	return (true);
}

/******************setters*********************/

void Authenticator::setNick(const std::string &nick)
{
	this->_nick = nick;
}

void Authenticator::setUser(const std::string &user)
{
	this->_user = user;
}

void Authenticator::setRealName(const std::string &realName)
{
	this->_realName = realName;
}
