/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 07:34:34 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/18 11:56:42 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

int    JOIN_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    size_t p = 0;
    int flag = -1;
    p_c command = splite_str(cmds);
    std::string buffer;
    if (cmds.size() >= 2) //check the size if there is something after join
    {
        for (size_t i = 0; i < command.channels_name.size();i++) //iterate on the channel names
        {
            flag = -1;
            if ((command.channels_name[i][0] == '#' || command.channels_name[i][0] == '&') && command.channels_name[i].size() > 1 && !strchr(" \a\0\n\r,", command.channels_name[i][1])) // check if this channel is valid first
            {
                flag = doesChannelExist(channels, command.channels_name[i]);
                if (flag == -1) // the channel doesn't exist create it and join it
                    create_join_channel(channels, command, i, client);
                else {
                    if (in_channel(channels[flag], client.nickName)) // most important check if u already in the channel
                        continue ; // if we return that's because our user is in the channel and we should do nothing
                    if (isHeInvited(channels[flag], client.nickName)){//check if he in the invite list if yes he need to join the channel immediately
                        join_channel(channels, flag, client); // he was invited he should join no matter what
                        continue ;
                    }
                    if (channels[flag].is_invite_only){ // check if the channel is invite only
                        buffer = ":ircserver 473 " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel, you must be invited (+i)\r\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                        continue ;
                    }
                    if (channels[flag].is_limit){
                        if (channels[flag].members_limit <= channels[flag].members.size()){
                            buffer = ":ircserver 471 " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+l)\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                            continue ;
                        }
                        else{
                            if (channels[flag].is_key){
                                if (command.channels_key.size() >= p + 1 && channels[flag].channel_key == command.channels_key[p]){
                                    p++;
                                    join_channel(channels, flag, client);
                                }
                                else{
                                    p++;
                                    buffer = ":ircserver 475 " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel, you need the correct key (+k)\r\n";
                                    send(fd, buffer.c_str(), buffer.length(), 0);
                                }
                            }
                            else{
                                join_channel(channels, flag, client);
                            }
                        }
                    }
                    else if (channels[flag].is_key){
                        if (command.channels_key.size() >= (p + 1) && channels[flag].channel_key == command.channels_key[p]){
                            p++;
                            join_channel(channels, flag, client);
                        }
                        else{
                            p++;
                            buffer = ":ircserver 475 " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel, you need the correct key (+k)\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else{
                        join_channel(channels, flag, client);
                    }
                }
            }
            else{
                buffer = ":ircserver 403 " + cmds[0] + " :No such channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    else{
        buffer = ":ircserver 461 " + client.nickName + " " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}
