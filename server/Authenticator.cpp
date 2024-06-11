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

std::string &Authenticator::stringTrim(std::string &str, const char c)
{
	// while ()
	return ;
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
	while (str[i] != '/0')
	{
		if (islower(static_cast<int> (str[i])))
			str[i] = toupper(static_cast<int> (str[i]));
		i++;
	}
}

int Authenticator::checkPassword(const Server &obj)
{
	std::string pass;
	std::string password;
	std::string thirdParam;

	// if (this->_isPassSet)
	// {
	// 	obj.sendMsg("ERR_ALREADYREGISTERED (462)", obj.getClientFd());
	// 	return (true);
	// }
	std::getline(this->_stringStream, pass, ' ');
	std::getline(this->_stringStream, password, ' ');
	std::getline(this->_stringStream, thirdParam, ' ');
	if (thirdParam.length() > 0)
	{
		obj.sendMsg("TOO MANY PARAMS ()", obj.getClientFd());
	}
	if (pass.length() == 0 || password.length() == 0)
	{
		obj.sendMsg("ERR_NEEDMOREPARAMS (461)", obj.getClientFd());
	}
	this->toUpper(pass);
	if (pass.compare("PASS") != 0)
	{
		obj.sendMsg("ERR_NOTREGISTERED (451)", obj.getClientFd());
		return (false);
	}
	if (obj.getPassword().compare(password) == 0)
		return (true);
	obj.sendMsg("ERR_PASSWDMISMATCH (464)", obj.getClientFd());
	return (false);
}

int Authenticator::checkNick(const Server &obj)
{
	
    return 0;
}

int Authenticator::checkUser(const Server &obj)
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
	if (sixthParam.length() > 0)
	{
		obj.sendMsg("TOO MANY PARAMS IN USER()", obj.getClientFd());
		return (false);
	}
	if (!firstParam.length() || !secondParam.length() || !thirdParam.length() || !fourthParam.length() || !fifthParam.length())
	{
		obj.sendMsg("ERR_NEEDMOREPARAMS (461)", obj.getClientFd());
		return (false);
	}
	this->setUser(secondParam);
	this->setRealName(fifthParam);
    return (true);
}

int Authenticator::checkClientAuthentication(const Server &obj)
{
	Client client;
	Authenticator auth;
	std::string line;

	//check is the password that entered is corect
	do
	{
		std::getline(std::cin, line);
		if (line.length() == 0)
			continue ;
		auth.pushLineToStream(line);
		for (int i = 0; i < 4; i++)
			line[i] = toupper(static_cast<int>(line[i]));
		if (line.compare(0, 4, "PASS ", 0, 4) == 0)
			auth._isPassSet = auth.checkPassword(obj);
		else if (auth._isPassSet && line.compare(0, 4, "USER ", 0, 4) == 0)
			auth._isUserSet = auth.checkUser(obj);
		else if (auth._isPassSet && line.compare(0, 4, "NICK ", 0, 4) == 0)
			auth._isNickSet = auth.checkNick(obj);
		else
			obj.sendMsg("ERR_NOTREGISTRED (451)", obj.getClientFd());
		line.clear();
		line = "";
	} while (!auth._isNickSet || !auth._isUserSet || !auth._isPassSet);
	
	
	//check is the nick or the user correct

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
