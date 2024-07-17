/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 07:33:27 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/17 12:13:54 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

int PRIVMSG_COMMAND(int fd, std::vector<std::string> &cmds, std::map<int, Client> &mapo, std::vector<Channels> &channels){
    int flag = -1;
    std::string buffer;
    std::string buffer1;
    if (cmds.size() >= 3)
    {
        if ((cmds[1][0] == '#' || cmds[1][0] == '&') && check_channel(fd, channels, cmds[1], mapo[fd]) >= 0){
            flag = check_channel(fd, channels, cmds[1], mapo[fd]);
            if (imInChannel(channels[flag], mapo[fd].nickName)){ // check if u are in the channel if yes then u can send the message in the channel
                for (size_t j = 2;j < cmds.size();j++){
                    buffer1 += cmds[j];
                    if (j+1 < cmds.size())
                        buffer1 += " ";
                }
                for (std::map<std::string, int>::iterator it =  channels[flag].members.begin();it != channels[flag].members.end();it++){
                    if (it->second == fd)
                        continue ;
                    buffer = ":" + mapo[fd].nickName + "!" + mapo[fd].userName + mapo[fd].ip + " PRIVMSG " + cmds[1] + " "+ buffer1 + "\r\n";
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
            if (cmds[1][0] == '#' )
                return 0;
            for (std::map<int, Client>::iterator ito = mapo.begin(); ito != mapo.end(); ito++){ //search for the user in the server members
                if (ito->second.nickName == cmds[1]){ //if u find him send him the message
                    flag = 1;
                    for (size_t j = 2;j < cmds.size();j++){
                        buffer += cmds[j];
                        if (j+1 < cmds.size())
                            buffer += " ";
                    }
                    buffer = ":" + mapo[fd].nickName + "!" + mapo[fd].userName + "@" + mapo[fd].ip + " PRIVMSG " + cmds[1] + " " + buffer + "\r\n";
                    send(ito->second.fd, buffer.c_str(), buffer.length(), 0);
                    break;
                }
            }
            if (!flag){ //if u didn't find him send this error
                buffer = ":ircserver 401 " + cmds[2] + " No such nick\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    else{
        buffer = ":ircserver 461 " + mapo[fd].nickName + " " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}
