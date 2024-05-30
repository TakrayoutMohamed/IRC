/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:25:55 by mel-jira          #+#    #+#             */
/*   Updated: 2024/05/30 21:10:08 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

Client::Client(){
    nickname = "";
    password = "";
    ip = "";
}

Channels::Channels()
{
    channel_name = "";
    channel_topic = "";
    channel_key = "";
    is_invite_only = false;
    is_limit = false;
    members_limit = 0;
}
// in this function i splite the user input to command "string" and a
// vector of string where i store the channels name and a
// vector of string where i store the channels key

std::vector<std::string> split(std::string &str, char sep){
    std::vector<std::string> names;
    std::stringstream ss(str);
    std::string name;
    
    while (std::getline(ss, name, sep)) {
        names.push_back(name);
    }
    return (names);
}

p_c splite_str(std::vector<std::string> &str)
{
    p_c command;

    command.command = str[0];
    command.channels_name = split(str[1], ',');
    if (str.size() > 2)
        command.channels_key = split(str[2], ',');
    return (command);
}

void    create_join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client){
    Channels channel;
    std::string buffer;
    channel.channel_name = command.channels_name[index];
    channel.channel_create_time = time(0);
    channel.members.insert(std::make_pair(client.nickname, client.fd));
    channel.admin_list.push_back(client.nickname);
    client.inside_channel.push_back({command.channels_name[index], true});
    channels.push_back(channel);
    buffer = ":" + client.nickname + "!~a@" + client.ip + " JOIN" + command.channels_name[index];
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 353" + client.nickname + " = " + command.channels_name[index] + " :@" + client.nickname + "\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 366" + client.nickname + " " + command.channels_name[index] + " :End of /NAMES list.\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
}

void    join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client)
{
    std::string buffer;
    channels[index].members.insert(std::make_pair(client.nickname, client.fd));
    if (channels[index].is_invite_only)
    {
        for (int i = 0;i < channels[index].invite_list.size();i++)
        {
            if (channels[index].invite_list[i] == client.nickname)
            {
                channels[index].invite_list[i].erase();
                break ;
            }
        }
    }
    for (std::map<std::string, int>::iterator it =  channels[index].members.begin();it != channels[index].members.end();it++)
    {
        buffer = ":" + client.nickname + "!~a@" + client.ip + " JOIN" + command.channels_name[index];
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 353" + client.nickname + " = " + command.channels_name[index] + " :@" + channels[index].invite_list[0];
        for (int j = 1;j < channels[index].invite_list.size();j++)
        {
            buffer += channels[index].invite_list[0];
            if (j+1 < channels[index].invite_list.size())
                buffer += " ";
        }
        buffer += "\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 366" + client.nickname + " " + command.channels_name[index] + " :End of /NAMES list.\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
    }
}

int    JOIN_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    int flag = -1;
    p_c command = splite_str(cmds);
    std::string buffer;
    if (cmds.size() >= 2)
    {
        for (int i = 0; i < command.channels_name.size();i++) 
        {
            flag = -1;
            if (command.channels_name[i][0] == '#')
            {
                for (int j = 0; i < channels.size();i++)
                {
                    if (command.channels_name[i] == channels[j].channel_name)
                    {
                        flag = j;
                        break ;
                    }
                }
                if (flag == -1)
                {
                    create_join_channel(channels, command, i, client);
                }
                else
                {
                    // the channel does exist
                    if (channels[flag].is_invite_only)
                    {
                        for (int j = 0; j < channels[flag].invite_list.size();j++)
                        {
                            if (client.nickname == channels[flag].invite_list[j])
                            {
                                if (channels[flag].is_limit && channels[flag].members_limit >= channels[flag].members.size())
                                {
                                    //we exceed the limit fo the member in the channels
                                    buffer = "Error(743): " + client.nickname + " " + channels[flag].channel_name + " :Cannot join channel (+l)\n";
                                    send(fd, buffer.c_str(), buffer.length(), 0);
                                    break ;
                                }
                                //he is in the invite list
                                if (!channels[flag].channel_key.empty()) // channel has a key
                                {
                                    if (cmds.size() >= 3)
                                    {
                                        if (channels[flag].channel_key == command.channels_key[p])
                                        {
                                            //key is correct join the channel
                                            join_channel(channels, command, flag, client);
                                        }
                                        else
                                        {
                                            buffer = "Error(475): " + client.nickname + " " + channels[flag].channel_name + " :Cannot join channel (+k)\n";
                                            send(fd, buffer.c_str(), buffer.length(), 0);
                                            break ;
                                        }
                                    }
                                }
                                else
                                {
                                    // join the channel because it doesn't have a key
                                    join_channel(channels, command, flag, client);
                                }
                            }
                            if (j+1 > channels[flag].invite_list.size())
                            {
                                buffer = "Error(473): " + client.nickname + " " + channels[flag].channel_name + " :Cannot join channel (+i)\n";
                                send(fd, buffer.c_str(), buffer.length(), 0);
                                break ;
                            }
                        }
                    }
                    else
                    {
                        //check limit
                        if (channels[flag].is_limit && channels[flag].members_limit >= channels[flag].members.size())
                        {
                            //we exceed the limit of the member in the channels
                            buffer = "Error(471): " + client.nickname + " " + channels[flag].channel_name + " :Cannot join channel (+l)\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].channel_key.empty()) // channel has a key
                        {
                            if (cmds.size() >= 3)
                            {
                                if (channels[flag].channel_key == command.channels_key[p])
                                {
                                    //key is correct join the channel join the channel
                                    join_channel(channels, command, flag, client);
                                }
                                else
                                {
                                    //  "<client> <channel> :Cannot join channel (+k)"
                                    buffer = "Error(475): " + channels[flag].channel_name + " :Cannot join channel (+k)\n";
                                    send(fd, buffer.c_str(), buffer.length(), 0);
                                }
                            }
                        }
                        else
                        {
                            // join the channel because it doesn't have a key
                            join_channel(channels, command, flag, client);
                        }
                    }
                }
            }
            else
            {
                buffer = "Error(403): " + cmds[0] + " :No such channel\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    return 0;
}

void set_mode(Mode *mode)
{
    mode->l = "";
    mode->ml = 0;
    mode->pl = 0;
    mode->k = "";
    mode->mk = 0;
    mode->pk = 0;
    mode->o = "";
    mode->mo = 0;
    mode->po = 0;
    mode->i = "";
    mode->mi = 0;
    mode->pi = 0;
    mode->t = "";
    mode->mt = 0;
    mode->pt = 0;
}

int     MODE_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    Mode mode;
    int sign = 0;
    int op = 0;
    set_mode(&mode);
    int flag = -1;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        //check if channel u wanna mode exist
        std::map<std::string, int>::iterator it = channels[flag].members.find(cmds[1]);
        if (it != channels[flag].members.end())
        {
            
        }
        else
        {
            buffer = "Error(403): " + cmds[1] + " :No such channel\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
}

//kick is done
int KICK_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    int flag = -1;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        for (int i = 0;i < channels.size();i++)
        {
            if (cmds[1] == channels[i].channel_name)
            {
                flag = i;
                break ;
            }
        }
        if (flag == -1)
        {
            buffer = "Error(403): " + cmds[1] + " :No such channel\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
        else
        {
            for (int i = 0;i< client.inside_channel.size();i++)
            {
                if (channels[flag].admin_list[i] == client.nickname)
                {
                    std::map<std::string, int>::iterator it = channels[flag].members.find(cmds[2]);
                    if (it != channels[flag].members.end())
                    {
                        buffer = "You have been kicked out of the channel\n";
                        send(it->second , buffer.c_str(), buffer.length(), 0);
                        for (std::map<std::string, int>::iterator ito = channels[flag].members.begin(); it != channels[flag].members.end(); ito++) {
                            if (cmds.size() <= 3)
                                buffer = client.nickname + " has kicked " + cmds[2] + " (" + client.nickname + ")\n";
                            else
                                buffer = client.nickname + " has kicked " + cmds[2] + " (" + cmds[3] + ")\n";
                            send(ito->second, buffer.c_str(), buffer.length(), 0);
                        }
                        channels[flag].members.erase(it);
                        for (int j = 0;j < channels[flag].admin_list.size();j++)
                        {
                            if (cmds[2] == channels[flag].admin_list[j])
                            {
                                channels[flag].admin_list[j].erase();
                            }
                        }
                    }
                    else
                    {
                        buffer = "Error(441): " + cmds[2] + " " + channels[flag].channel_name + " They aren't on that channel\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                    }
                }
                if (i+1 == client.inside_channel.size())
                {
                    buffer = "Error(482): " + channels[flag].channel_name + " :You're not channel operato\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
            }
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
}

int INVITE_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    int flag = -1;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        if (cmds[1][0] == '#')
        {
            //send it in a channel
            
        }
        else
        {
            //send it to a person
            
        }
    }
}

int TOPIC_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){

}

int PRIVMSG_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){

}

int IS_COMMAND_VALID(int fd, std::string &str, std::map<int, Client> &mapo, std::vector<Channels> channels)
{
    std::vector<std::string> cmds;
    cmds = split(str, ' ');
    std::string cmd = cmds[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::string buffer;
    if (cmds.size() >= 2)
    {
        if (cmd == "JOIN")
        {
            if (JOIN_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else if (cmd == "MODE")
        {
            if (MODE_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else if (cmd == "KICK")
        {
            if (KICK_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else if (cmd == "INVITE")
        {
            if (INVITE_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else if (cmd == "TOPIC"){
            if (TOPIC_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else if (cmd == "PRIVMSG")
        {
            if (PRIVMSG_COMMAND(fd, cmds, mapo[fd], channels));
                return 1;
        }
        else
        {
            buffer = "Error(421): " + cmds[0] + " Unknown command\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return (0);
}
