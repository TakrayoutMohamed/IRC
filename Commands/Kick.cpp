/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 07:31:50 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/18 12:32:55 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

//need to remove the channel name from the channels he is inside
int KICK_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    int kicked = 0;
    int flag = -1;
    std::string buffer;
    std::string reason;
    if (cmds.size() >= 3)
    {
        for (size_t i = 0;i < channels.size();i++)
        {
            if (cmds[1] == channels[i].channel_name){
                flag = i;
                break ;
            }
        }
        if (flag == -1){
            buffer = ":ircserver 403 " + cmds[1] + " :No such channel\r\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
        else{
            if (!is_admin(channels[flag], client.nickName)){
                buffer = ":ircserver 482 " + client.nickName + " " + channels[flag].channel_name + " :You're not channel operato\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
                return 1;
            }
            if (in_channel(channels[flag], cmds[2])){
                if (cmds.size() <= 3){
                    buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " KICK " + cmds[1] + " " + cmds[2] + " :" + cmds[2] + "\r\n";
                }
                else{
                    for (size_t i = 3;i < cmds.size();i++){
                        reason += cmds[i];
                        if (i+1 < cmds.size())
                            reason += " ";
                    }
                    buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " KICK " + cmds[1] + " " + cmds[2] + " :" + reason + "\r\n";
                }
                broad_cast(channels[flag], buffer, "", "");
                channels[flag].members.erase(cmds[2]);
                channels[flag].members_count--;
                kicked = 1;
            }
            if (is_admin(channels[flag], cmds[2])){
                for (size_t j = 0;j < channels[flag].admin_list.size();j++){
                    if (cmds[2] == channels[flag].admin_list[j]){
                        channels[flag].admin_list[j].erase();
                        kicked = 1;
                        break ;
                    }
                }
            }
            if (kicked){
                for (std::vector<std::string>::iterator it = client.inside_channel.begin();it != client.inside_channel.end();it++){
                    if (*it == channels[flag].channel_name){
                        client.inside_channel.erase(it);
                        break ;
                    }
                }
            }
            if (!kicked){
                buffer = ":ircserver 441 " + cmds[2] + " " + channels[flag].channel_name + " They aren't on that channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    else
    {
        buffer = ":ircserver 461 " + client.nickName + " " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}
