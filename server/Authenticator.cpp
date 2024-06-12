#include "./Authenticator.hpp"
#include "Authenticator.hpp"
#include <iostream>

Authenticator::Authenticator(/* args */) : _nick(""), _user(""), _realName("")
{
	_isPassSet = false;
	_isNickSet = false;
	_isUserSet = false;
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

int Authenticator::checkPassword(const Server &server)
{
	std::string pass;
	std::string password;
	std::string thirdParam;

	// if (this->_isPassSet)
	// {
	// 	server.sendMsg("ERR_ALREADYREGISTERED (462)", server.getClientFd());
	// 	return (true);
	// }
	std::getline(this->_stringStream, pass, ' ');
	std::getline(this->_stringStream, password, ' ');
	std::getline(this->_stringStream, thirdParam, ' ');
	if (thirdParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS ()", server.getClientFd());
	}
	if (pass.length() == 0 || password.length() == 0)
	{
		server.sendMsg("ERR_NEEDMOREPARAMS (461)", server.getClientFd());
	}
	this->toUpper(pass);
	if (pass.compare("PASS") != 0)
	{
		server.sendMsg("ERR_NOTREGISTERED (451)", server.getClientFd());
		return (false);
	}
	if (server.getPassword().compare(password) == 0)
		return (true);
	server.sendMsg("ERR_PASSWDMISMATCH (464)", server.getClientFd());
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
		if (it->second.hostName.compare(nick) == 0)
			return (true);
		it++;
	}
	return (false);
}

int Authenticator::checkNick(const Server &server)
{
	std::string firstParam;
	std::string secondParam;

	if (this->_isPassSet == false)
	{
		server.sendMsg("YOU NEED TO SET THE SERVER'S PASSWORD FIRST", server.getClientFd());
		return (false);
	}	
	std::getline(this->_stringStream, firstParam, ' ');
	std::getline(this->_stringStream, secondParam, ' ');
	if (hasUnacceptedChars(secondParam))
	{
		server.sendMsg("ERR_ERRONEUSNICKNAME (432)", server.getClientFd());
		return (false);
	}
	if (secondParam.length() == 0 || secondParam.length() == 0)
	{
		server.sendMsg("ERR_NONICKNAMEISGIVEN (431)", server.getClientFd());
		return (false);
	}
	this->toUpper(firstParam);
	if (firstParam.compare("NICK") != 0)
	{
		server.sendMsg("ERR_NOTREGISTERED (451)", server.getClientFd());
		return (false);
	}
	// if (std::find(server.getData().begin(), secondParam) != server.getData().end())
	if (isNickNameInUse(server.getData(), secondParam))
	{
		server.sendMsg("ERR_NICKNAMEINUSE (433)", server.getClientFd());
		return (false);
	}
	this->setNick(secondParam);
	return (true);
}

int Authenticator::checkUser(const Server &server)
{
	std::string firstParam;
	std::string secondParam;
	std::string thirdParam;
	std::string fourthParam;
	std::string fifthParam;
	std::string sixthParam;

	if (this->_isPassSet == false)
	{
		server.sendMsg("YOU NEED TO SET THE SERVER'S PASSWORD FIRST", server.getClientFd());
		return (false);
	}
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
	if (sixthParam.length() > 0)
	{
		server.sendMsg("TOO MANY PARAMS IN USER()", server.getClientFd());
		return (false);
	}
	if (!firstParam.length() || !secondParam.length() || !thirdParam.length() || !fourthParam.length() || !fifthParam.length())
	{
		server.sendMsg("ERR_NEEDMOREPARAMS (461)", server.getClientFd());
		return (false);
	}
	this->setUser(("~" + secondParam).c_str());
	this->setRealName(fifthParam);
    return (true);
}

int Authenticator::checkClientAuthentication(Server &server, Client &client)
{
	Authenticator auth;
	std::string line;
	char msg[1024];

	do
	{
		// std::cout << "testing: here is the checkclientauthentication " << std::endl;
		// server.sendMsg("testing: here is the checkclientauthentication ", server.getClientFd());
		// std::getline(std::cin, line);
		recv(client.fd, msg, 1024, 0);
		line = msg;
		if (line.length() == 0)
			continue ;
		auth.pushLineToStream(line);
		for (int i = 0; i < 4; i++)
			line[i] = toupper(static_cast<int>(line[i]));
		if (line.compare(0, 4, "PASS ", 0, 4) == 0)
			auth._isPassSet = auth.checkPassword(server);
		else if (line.compare(0, 4, "USER ", 0, 4) == 0)
			auth._isUserSet = auth.checkUser(server);
		else if (line.compare(0, 4, "NICK ", 0, 4) == 0)
			auth._isNickSet = auth.checkNick(server);
		else if (line.length() > 0)
			server.sendMsg("ERR_NOTREGISTRED (451)", server.getClientFd());
		line.clear();
		line = "";
	} while (!auth._isNickSet || !auth._isUserSet || !auth._isPassSet);
	client.nickName = auth._nick;
	client.realName = auth._realName;
	client.userName = auth._user;
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
