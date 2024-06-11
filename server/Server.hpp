#ifndef SERVER_HPP
# define SERVER_HPP
# define TOOMANYPARAMS 2
# define NEEDMOREPARAMS 3
# define PASSWORDMISMATCH 4

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
# include <vector>
# include <map>
# include "./Authenticator.hpp"
# include "./../exception/PasswordNotAcceptedException.hpp"
# include "./Client.hpp"

// class Client;
class Server
{
	private:
		std::string _password;
		int _port;
		int	_serverSocket;
		unsigned int _clientFd;
		std::vector<pollfd> _socketsFds;
		std::map<int, Client> _data;
		socklen_t _clientLen, _serverLen;
		struct sockaddr_in  _clientAddr, _serverAddr;

		bool	isValidPassword(const std::string &) const;
		bool	isValidPort(const int) const;
	
	protected:
		/*setter member functions*/
		void setPassword(const std::string &);
		void setPort(const int &);
	
		Server(const Server &obj); //till now no need 
		const Server &operator=(const Server &obj);//till now no need

		/*EXCEPTIONS : INNER CLASSES*/
		class	PortNotAcceptedException : public std::exception
		{
			virtual const char *what() const throw();
		} PortNotAcceptedException;
	public:
		Server(); //till now no need
		Server(const std::string &passwd, const std::string &port);
		~Server();//till now no need
		static void	runServer(const std::string &password, const std::string &port);
		void sendMsg(const std::string &msg, int fd) const;
		void addData(int fd, const Client &client);

		/*setter member functions*/
		
		/*getter member functions*/
		const std::string &getPassword(void) const;
		const int &getPort(void) const;
		const unsigned int &getClientFd(void) const;
		const std::map<int, Client> &getData(void) const;
	/************************* Methods ************************/
	private:
		void	openSocket(void);
		int		bindSocket(void);
		int		listenSocket(void);
		/// @brief runing poll() system call to check if there are new ready file descriptors 
		/// @return the number of ready file descriptors
		int		checkFdsForNewEvents(void);
		int		acceptClientSocket(void);
		int		saveClientData(void);
		int		deleteClientFd(int fd);
		int		readClientFd(int fd);

};

int	convertStringToInt(const std::string &str);
bool	hasSpace(const std::string &str);
#endif
