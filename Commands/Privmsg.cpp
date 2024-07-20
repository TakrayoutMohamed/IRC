/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 07:33:27 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/20 21:11:15 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

int PRIVMSG_COMMAND(int fd, std::vector<std::string> &cmds, std::map<int, Client> &mapo, std::vector<Channels> &channels){
    int flag = -1;
    p_c command = splite_message(cmds);
    std::string buffer;
    std::string buffer1;
    if (cmds.size() < 3)
    {
        buffer = ":ircserver 461 " + mapo[fd].nickName + " " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
        return 1;
    }
    for (size_t i = 0; i < command.channels_name.size();i++) //iterate on the channels or names
    {
        flag = -1;
        if ((command.channels_name[i][0] == '#' || command.channels_name[i][0] == '&') && doesChannelExist(channels, command.channels_name[i]) >= 0)
        {
            flag = doesChannelExist(channels, command.channels_name[i]);
            if (imInChannel(channels[flag], mapo[fd].nickName)){
                buffer1 = command.channels_key[0];
                for (std::map<std::string, int>::iterator it =  channels[flag].members.begin();it != channels[flag].members.end();it++){
                    if (it->second == fd)
                        continue ;
                    buffer = ":" + mapo[fd].nickName + "!" + mapo[fd].userName + "@" + mapo[fd].ip + " PRIVMSG " + cmds[1] + " :"+ buffer1 + "\r\n";
                    send(it->second, buffer.c_str(), buffer.length(), 0);
                }
            }
            else{
                buffer = ":ircserver 404 " + mapo[fd].nickName + " " + channels[flag].channel_name + " :Cannot send to channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
            
        }
        else //the message is to a user
        {
            if (command.channels_name[i][0] == '#' )
                return 0;
            for (std::map<int, Client>::iterator ito = mapo.begin(); ito != mapo.end(); ito++){ //search for the user in the server members
                if (ito->second.nickName == command.channels_name[i]){ //if u find him send him the message
                    flag = 1;
                    buffer1 = command.channels_key[0];
                    buffer = ":" + mapo[fd].nickName + "!" + mapo[fd].userName + "@" + mapo[fd].ip + " PRIVMSG " + command.channels_name[i] + " :" + buffer1 + "\r\n";
                    send(ito->second.fd, buffer.c_str(), buffer.length(), 0);
                    break;
                }
            }
            if (!flag){ //if u didn't find him send this error
                buffer = ":ircserver 401 " + command.channels_name[i] + " :No such nick/channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    return 0;
}
