#ifndef AUTHENTICATOR_HPP
# define AUTHENTICATOR_HPP
# include "./Server.hpp"
# include "./Client.hpp"
# include <iostream>


class Authenticator
{
	private:
		/* data */
		std::stringstream _stringStream;
		std::string _nick;
		std::string _user;
		std::string _realName;
		bool _isPassSet;
		bool _isNickSet;
		bool _isUserSet;
		
	public:
		Authenticator(/* args */);
		~Authenticator();
        std::string &stringTrim(std::string &str, const char c);
        std::stringstream &pushLineToStream(const std::string &line);
		void		toUpper(std::string &str);
        int			checkPassword(const Server &obj);
        int			checkNickUser(const Server &obj);
		int			checkNick(const Server &obj);
		int			checkUser(const Server &obj);
        /// @brief 		this member meant to check if the client entered his information as it should (password , user , nick ,realname)
        /// @param obj  an obj of the server
        static int checkClientAuthentication(const Server &obj);
		/*setters*/

		void		setNick(const std::string &nick);
		void		setUser(const std::string &user);
		void		setRealName(const std::string &realName);

		/*getters*/
};
#endif
