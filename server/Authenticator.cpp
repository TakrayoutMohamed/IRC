#include "./Authenticator.hpp"
#include "Authenticator.hpp"
#include <iostream>

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

int Authenticator::checkPassword(const Server &server, int fd)
{
	std::string pass;
	std::string password;
	std::string thirdParam;

	std::getline(this->_stringStream, pass, ' ');
	std::getline(this->_stringStream, password, ' ');
	std::getline(this->_stringStream, thirdParam, ' ');
	std::cout << "pass = ["<< pass <<"]" << "password = ["<< password <<"]" << "thirdParam = ["<< thirdParam <<"]" << std::endl;
	if (thirdParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS ()", fd);
		return (false);
	}
	if (pass.length() == 0 || password.length() == 0)
	{
		server.sendMsg("ERR_NEEDMOREPARAMS (461)", fd);
		return (false);
	}
	this->toUpper(pass);
	if (pass.compare("PASS") != 0)
	{
		server.sendMsg("ERR_NOTREGISTERED (451)", fd);
		return (false);
	}
	if (server.getPassword().compare(password) == 0)
		return (true);
	server.sendMsg("ERR_PASSWDMISMATCH (464)", fd);
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

int Authenticator::checkNick(Server &server, int fd)
{
	std::string firstParam;
	std::string secondParam;
	std::string thirdParam;

	std::getline(this->_stringStream, firstParam, ' ');
	std::getline(this->_stringStream, secondParam, ' ');
	std::getline(this->_stringStream, thirdParam, ' ');
	std::cout << "nick = ["<< firstParam <<"]" << "nickname = ["<< secondParam <<"]" << std::endl;
	if (thirdParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS ()", fd);
		return (false);
	}
	if (hasUnacceptedChars(secondParam))
	{
		server.sendMsg("ERR_ERRONEUSNICKNAME (432)", fd);
		return (false);
	}
	if (secondParam.length() == 0 || secondParam.length() == 0)
	{
		server.sendMsg("ERR_NONICKNAMEISGIVEN (431)", fd);
		return (false);
	}
	this->toUpper(firstParam);
	if (firstParam.compare("NICK") != 0)
	{
		server.sendMsg("ERR_NOTREGISTERED (451)", fd);
		return (false);
	}
	if (isNickNameInUse(server.getData(), secondParam))
	{
		server.sendMsg("ERR_NICKNAMEINUSE (433)", fd);
		return (false);
	}
	this->setNick(secondParam);
	return (true);
}

int Authenticator::checkUser(const Server &server, int fd)
{
	std::string firstParam;
	std::string secondParam;
	std::string thirdParam;
	std::string fourthParam;
	std::string fifthParam;
	std::string sixthParam;

	std::getline(this->_stringStream, firstParam, ':');
	std::getline(this->_stringStream, fifthParam, ':');
	this->pushLineToStream(firstParam);
	firstParam.clear();
	firstParam = "";
	if (fifthParam.length() == 0)
	{
		std::getline(this->_stringStream, firstParam, ' ');
		std::getline(this->_stringStream, secondParam, ' ');
		std::getline(this->_stringStream, thirdParam, ' ');
		std::getline(this->_stringStream, fourthParam, ' ');
		std::getline(this->_stringStream, fifthParam, ' ');
		std::getline(this->_stringStream, sixthParam, ' ');
	}
	else
	{
		std::getline(this->_stringStream, firstParam, ' ');
		std::getline(this->_stringStream, secondParam, ' ');
		std::getline(this->_stringStream, thirdParam, ' ');
		std::getline(this->_stringStream, fourthParam, ' ');
		std::getline(this->_stringStream, sixthParam, ' ');

	}
	std::cout << "user = ["<< firstParam <<"]" << "userName = ["<< secondParam <<"]" << "servername = ["<< thirdParam <<"]" << "hostname = ["<< fourthParam << "]" << "realName = ["<< fifthParam << "]" ;
	std::cout << "sixthParam = ["<< sixthParam <<"]" << std::endl;
	if (sixthParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS IN USER()", fd);
		return (false);
	}
	if (!firstParam.length() || !secondParam.length() || !thirdParam.length() || !fourthParam.length() || !fifthParam.length())
	{
		server.sendMsg("ERR_NEEDMOREPARAMS (461)", fd);
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

int Authenticator::checkClientAuthentication(Server &server, Client &client, std::string &line)
{
	Authenticator auth;

	line.erase(line.end() - 1);
	auth.pushLineToStream(line);
	for (int i = 0; i < 4 && line[i] != '\0'; i++)
		line[i] = toupper(static_cast<int>(line[i]));
	if (line.compare(0, 4, "PASS ", 0, 4) != 0 && !client._isPassSet)
	{
		server.sendMsg("YOU NEED TO SET THE SERVER'S PASSWORD FIRST", client.fd);
		return (false);
	}
	if (line.compare(0, 4, "PASS ", 0, 4) == 0)
		client._isPassSet = auth.checkPassword(server, client.fd);
	else if (line.compare(0, 4, "USER ", 0, 4) == 0)
	{
		if (!client._isUserSet && auth.checkUser(server, client.fd))
		{
			client._isUserSet = true;
			client.userName = auth._user;
			client.realName = auth._realName;
		}
	}
	else if (line.compare(0, 4, "NICK ", 0, 4) == 0)
	{
		if (!client._isNickSet && auth.checkNick(server, client.fd))
		{
			client._isNickSet = true;
			client.nickName = auth._nick;
		}
	}
	else if (line.length() > 0)
	{
		server.sendMsg("ERR_NOTREGISTRED (451)", client.fd);
		printClientData(client);
	}
	std::cout << " is Nick set" << client._isNickSet << " is User set" << client._isUserSet << " is Pass set" << client._isPassSet <<  std::endl;
	if (client._isNickSet && client._isPassSet && client._isUserSet)
		client.isAuthenticated = true;
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
