#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <limits.h>      /* for OPEN_MAX */
# include <iostream>
# include <unistd.h> /*close()*/
# include <sys/socket.h> /*socket() , bind(), listen(), accept()*/
# include <arpa/inet.h> /*htons*/
# include <netinet/in.h> /*sockaddr_in*/
# include <netdb.h> /*gethostbyname()*/
# include <fcntl.h> /*fcntl()*/
# include <poll.h> /*poll()*/

class Server
{
	private:
		int _port;
		std::string _password;

		const bool	isValidPassword(const std::string &) const;
		const bool	isValidPort(const int) const;
	
	protected:
		/*setter functions*/
		void setPassword(const std::string &);
		void setPort(const int &);
	
	public:
		Server();
		Server(const Server &obj);
		Server(const std::string &, const int);
		const Server &operator=(const Server &obj);
		~Server();
};

#endif
