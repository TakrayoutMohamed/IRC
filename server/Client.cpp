#include "./Client.hpp"
#include "Client.hpp"

Client::Client(/* args */)
{
	nickName = "";
	userName = "";
	realName = "";
	hostName = "";
	serverName = "";
	fd = -2;
	ip = "";
}

const Client &Client::operator=(const Client &obj)
{
	if (this != &obj)
	{
		this->fd			= obj.fd;
		this->hostName  	= obj.hostName;
		this->ip			= obj.ip;
		this->nickName		= obj.nickName;
		this->realName		= obj.realName;
		this->serverName	= obj.serverName;
		this->userName		= obj.serverName;
	}
	return (*this);
}

Client::~Client()
{
}