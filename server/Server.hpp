#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <sstream>
# include <limits.h>      /* for OPEN_MAX */
# include <iostream>
# include <unistd.h> /*close()*/
# include <sys/socket.h> /*socket() , bind(), listen(), accept()*/
# include <arpa/inet.h> /*htons*/
# include <netinet/in.h> /*sockaddr_in*/
# include <netdb.h> /*gethostbyname()*/
# include <fcntl.h> /*fcntl()*/
# include <poll.h> /*poll()*/
# include "./../exception/PasswordNotAcceptedException.hpp"

class Server
{
	private:
		int _port;
		std::string _password;

		const bool	isValidPassword(const std::string &) const;
		const bool	isValidPort(const int) const;
	
	protected:
		/*setter member functions*/
		void setPassword(const std::string &);
		void setPort(const int &);
		/*getter member functions*/
		const std::string &getPassword(void) const;
		const int &getPort(void) const;
	
		Server(); //till now no need
		Server(const Server &obj); //till now no need 
		Server(const std::string &, const std::string &);
		const Server &operator=(const Server &obj);//till now no need
		~Server();//till now no need

		/*EXCEPTIONS : INNER CLASSES*/
		class	PortNotAcceptedException : public std::exception
		{
			virtual const char *what() const throw();
		} PortNotAcceptedException;
	public:
		void	runServer(const std::string &, const std::string &) const;
};

#endif
