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
# include <vector>
# include "./../exception/PasswordNotAcceptedException.hpp"

class Server
{
	private:
		std::string _password;
		int _port;
		int	_serverSocket;
		unsigned int _clientFd;
		std::vector<pollfd> _clientsFds;
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
		/*getter member functions*/
		const std::string &getPassword(void) const;
		const int &getPort(void) const;
	/************************* Methods ************************/
	private:
		void	openSocket(void);
		int		bindSocket(void);
		int		listenSocket(void);
		/// @brief runing poll() system call to check if there are new ready file descriptors 
		/// @return the number of ready file descriptors
		int		checkFdsForNewEvents(void);
		int		acceptClientSocket(void);
		int		saveClientFd(void);
		int		deleteClientFd(int fd);
		int		readClientFd(int fd);

};

int	convertStringToInt(const std::string &str);
bool	hasSpace(const std::string &str);
#endif
