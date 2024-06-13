#ifndef AUTHENTICATOR_HPP
# define AUTHENTICATOR_HPP
# include "./Server.hpp"
# include "./Client.hpp"
# include <iostream>

class Server;

class Authenticator
{
	private:
		/* data */
		std::stringstream _stringStream;
		std::string _nick;
		std::string _user;
		std::string _realName;
		
	public:
		Authenticator(/* args */);
		~Authenticator();
        std::stringstream &pushLineToStream(const std::string &line);
		void		toUpper(std::string &str);
        int			checkPassword(const Server &obj, int fd);
		int			checkNick(Server &obj, int fd);
		int			checkUser(const Server &obj, int fd);
        /// @brief 		this member meant to check if the client entered his information as it should (password , user , nick ,realname)
        /// @param obj  an obj of the server
        static int checkClientAuthentication(Server &server, Client &client, std::string &line);
		/*setters*/

		void		setNick(const std::string &nick);
		void		setUser(const std::string &user);
		void		setRealName(const std::string &realName);

		/*getters*/
};
#endif
