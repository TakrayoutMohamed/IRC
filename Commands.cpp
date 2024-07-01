/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:25:55 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/01 16:10:03 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

// Client::Client(){
//     nickName = "";
//     ip = "";
// }

std::string myto_string(long long value) {
    std::stringstream oss;
    oss << value;
    return oss.str();
}

Channels::Channels()
{
    channel_name = "";
    channel_topic = "";
    channel_key = "";
    is_invite_only = false;
    is_limit = false;
    is_key = false;
    is_topic = false;
    members_limit = 0;
    topic_time = 0;
}

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

void    lookfor(std::string look, std::vector<std::pair<std::string, std::string> > &changes)
{
    for (int i = 0;i < changes.size();i++)
    {
        if (changes[i].first == look)
        {
            changes.erase(changes.begin()+i);
            break ;
        }
    }
}

bool already_op(Channels &channel, std::string &name){
    for (int i = 0;i < channel.admin_list.size();i++){
        if (channel.admin_list[i] == name)
            return (true);
    }
    return (false);
}

bool    in_channel(Channels &channel, std::string &name){
    for (std::map<std::string, int>::iterator it =  channel.members.begin();it != channel.members.end();it++){
        if (it->first == name)
            return (true);
    }
    return (false);
}

void    remove_admin(Channels &channel, std::string &name){
    for (int i = 0;i < channel.admin_list.size();i++){
        if (channel.admin_list[i] == name)
        {
            channel.admin_list.erase(channel.admin_list.begin()+i);
            break ;
        }
    }
}

bool    is_admin(Channels &channel, std::string &name){
    for (int i = 0;i < channel.admin_list.size();i++){
        if (channel.admin_list[i] ==  name)
            return (true);
    }
    return (false);
}

void    remove_member(Channels &channel, std::string &name){
    for (std::map<std::string, int>::iterator it =  channel.members.begin();it != channel.members.end();it++){
        if (it->first == name)
        {
            channel.members.erase(it);
            break ;
        }
    }
}

int check_channel(int fd, std::vector<Channels> &channels, std::string &channel_name){
    int flag = -1;
    std::string buffer;
    if (channel_name[0] != '#')
        return (flag);
    for (int i = 0;i < channels.size();i++)
    {
        if (channel_name == channels[i].channel_name)
        {
            flag = i;
            break ;
        }
    }
    if (flag == -1)
    {
        buffer = "Error(403): " + channel_name + " :No such channel\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return (flag);
}

void    broad_cast(Channels &channel, std::string error, std::string mid, std::string message){
    std::string buffer;
    for (std::map<std::string, int>::iterator it =  channel.members.begin();it != channel.members.end();it++)
    {
        buffer = error + mid + message;
        send(it->second, buffer.c_str(), buffer.length(), 0);
    }
}

void    create_join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client){
    Channels channel;
    std::string buffer;
    channel.channel_name = command.channels_name[index];
    channel.channel_create_time = time(0);
    channel.members.insert(std::make_pair(client.nickName, client.fd));
    channel.admin_list.push_back(client.nickName);
    client.inside_channel.push_back(std::make_pair(command.channels_name[index], true));
    channels.push_back(channel);
    buffer = ":" + client.nickName + "!~a@" + client.ip + " JOIN " + command.channels_name[index];
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 353 " + client.nickName + " @ " + command.channels_name[index] + " :@" + client.nickName + "\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 366 " + client.nickName + " " + command.channels_name[index] + " :End of /NAMES list.\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
}

void    join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client)
{
    std::string buffer;
    channels[index].members.insert(std::make_pair(client.nickName, client.fd));
    if (channels[index].is_invite_only)
    {
        for (int i = 0;i < channels[index].invite_list.size();i++)
        {
            if (channels[index].invite_list[i] == client.nickName)
            {
                channels[index].invite_list[i].erase();
                break ;
            }
        }
    }
    for (std::map<std::string, int>::iterator it =  channels[index].members.begin();it != channels[index].members.end();it++)
    {
        buffer = ":" + client.nickName + "!~a@" + client.ip + " JOIN " + command.channels_name[index];
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 353" + client.nickName + " @ " + command.channels_name[index] + " :@" + channels[index].invite_list[0];
        for (int j = 1;j < channels[index].invite_list.size();j++)
        {
            buffer += channels[index].invite_list[0];
            if (j+1 < channels[index].invite_list.size())
                buffer += " ";
        }
        buffer += "\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 366 " + client.nickName + " " + command.channels_name[index] + " :End of /NAMES list.\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
    }
}

//if the someone was invited he can join without a key and even if the channel is in the limit
//if the user is not invited u check if the channel invite only and if it exceed the limit of channel members
int    JOIN_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    int p = 2;
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
                            if (client.nickName == channels[flag].invite_list[j])
                            {
                                if (channels[flag].is_limit && channels[flag].members_limit >= channels[flag].members.size())
                                {
                                    //we exceed the limit fo the member in the channels
                                    buffer = "Error(743): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+l)\n";
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
                                            buffer = "Error(475): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+k)\n";
                                            send(fd, buffer.c_str(), buffer.length(), 0);
                                            break ;
                                        }
                                        p++;
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
                                buffer = "Error(473): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+i)\n";
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
                            buffer = "Error(471): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+l)\n";
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

int     MODE_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    int arg = 0;
    char sign = '+';
    std::string error = "i";
    int flag = -1;
    std::string buffer1 = "";
    std::string buffer = "";
    std::vector<std::pair<std::string, std::string> > changes;
    int key = 3;
    int x = 0;
    if (cmds.size() >= 2)
    {
        //check if channel u wanna mode exist
        std::map<std::string, int>::iterator it = channels[flag].members.find(cmds[1]);
        if (it != channels[flag].members.end())
        {
            if (cmds.size() >= 3)
            {
                //the channel exist do your parsing
                for (int i = 0;cmds[2][i];i++)
                {
                    while (cmds[2][i] && (cmds[2][i] == '+' || cmds[2][i] == '-'))
                    {
                        sign = cmds[2][i];
                        i++;
                    }
                    if (sign == '+' && cmds[2][i])
                        buffer1 += sign;
                    else if (sign == '-' && cmds[2][i])
                        buffer1 += sign;
                    if (strchr("lkito", cmds[2][i]))
                        buffer1 += cmds[2][i];
                    else
                    {
                        error = cmds[2][i];
                        buffer1 = "servername Error(472): " + error + " is an unknown mode char to me\n";
                        return 0;
                    }
                }
                for (int i = 0; buffer1[i]; i += 2)
                {
                    if (buffer1[i] && buffer1[i+1] && buffer1[i] == '+' && strchr("kl", buffer1[i+1]))
                        arg++;
                    else if (buffer1[i] && buffer1[i+1] && buffer1[i] == '-' && strchr("k", buffer1[i+1]))
                        arg++;
                }
                if (arg > cmds.size() -2)
                {
                    buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                    return 0;
                }
                for (int i = 0;i < buffer1.length();i +=2)
                {
                    if (!strncmp(&buffer1[i], "+i", 2))
                    {
                        if (channels[flag].is_invite_only)
                        {
                            buffer = "channel is already set to invite only: on\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_invite_only)
                        {
                            lookfor("-i", changes);
                            changes.push_back(std::make_pair("+i", ""));
                            channels[flag].is_invite_only = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-i", 2))
                    {
                        if (channels[flag].is_invite_only)
                        {
                            lookfor("+i", changes);
                            changes.push_back(std::make_pair("-i", ""));
                            channels[flag].is_invite_only = false;
                        }
                        else if (!channels[flag].is_invite_only)
                        {
                            buffer = "channel is already set to invite only: off\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+t", 2))
                    {
                        if (channels[flag].is_topic)
                        {
                            buffer = "channel is already set to topic: on\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_topic)
                        {
                            lookfor("-t", changes);
                            changes.push_back(std::make_pair("+t", ""));
                            channels[flag].is_topic = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-t", 2))
                    {
                        if (channels[flag].is_topic)
                        {
                            lookfor("+t", changes);
                            changes.push_back(std::make_pair("-t", ""));
                            channels[flag].is_topic = false;
                            channels[flag].channel_topic = "";
                        }
                        else if (!channels[flag].is_topic)
                        {
                            buffer = "channel is already set to topic: off\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+k", 2))
                    {
                        if (channels[flag].is_key)
                        {
                            buffer = "ircserver: error(467) " + client.nickName + " " + channels[flag].channel_name + " ::Channel key already set\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_key)
                        {
                            lookfor("-k", changes);
                            changes.push_back(std::make_pair("+k", cmds[key]));
                            channels[flag].channel_key = cmds[key++];
                            channels[flag].is_key = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-k", 2))
                    {
                        if (channels[flag].is_key)
                        {
                            if (channels[flag].channel_key == cmds[key])
                            {
                                lookfor("+k", changes);
                                changes.push_back(std::make_pair("-k", ""));
                                channels[flag].is_key = false;
                                channels[flag].channel_key = "";
                                key++;
                            }
                            else
                            {
                                buffer = "ircserver: error(467) " + client.nickName + " " + channels[flag].channel_name + " ::Channel key already set\n";
                                send(fd, buffer.c_str(), buffer.length(), 0);
                            }
                        }
                        else if (!channels[flag].is_key)
                        {
                            buffer = "ircserver: error(467) " + client.nickName + " " + channels[flag].channel_name + " ::Channel key already set\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+l", 2))
                    {
                        if (channels[flag].is_limit)
                        {
                            if (channels[flag].members_limit != atoi(cmds[key].c_str()) && atoi(cmds[key].c_str()) > 0)
                            {
                                lookfor("+l", changes);
                                changes.push_back(std::make_pair("+l", cmds[key].c_str()));
                                channels[flag].members_limit = atoi(cmds[key].c_str());
                                key++;
                            }
                        }
                        else if (!channels[flag].is_limit) 
                        {
                            lookfor("-l", changes);
                            changes.push_back(std::make_pair("+l", cmds[key].c_str()));
                            channels[flag].members_limit = atoi(cmds[key].c_str());
                            channels[flag].is_limit = true;
                            key++;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-l", 2))
                    {
                        if (channels[flag].is_limit)
                        {
                            lookfor("+l", changes);
                            changes.push_back(std::make_pair("-l", ""));
                            channels[flag].is_limit = false;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+o", 2))
                    {
                        
                        //check if is he in the channel
                        if (in_channel(channels[flag], cmds[key]))
                        {    
                            if (already_op(channels[flag], cmds[key]))
                            {
                                //already admin ignore it
                                key++;
                            }
                            else
                            {
                                //add him to the admin list
                                changes.push_back(std::make_pair("+o", cmds[key]));
                                channels[flag].admin_list.push_back(cmds[key++]);
                            }
                        }
                        else
                        {
                            //he is not in the channel
                            buffer = "ircserver: error(401) " + client.nickName + " " + cmds[key] + " ::No such nick\n";
                            key++;
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-o", 2))
                    {
                        if (in_channel(channels[flag], cmds[key]))
                        { 
                            if (!already_op(channels[flag], cmds[key]))
                            {
                                //he is not an op so do nothing
                                key++;
                            }
                            else if (already_op(channels[flag], cmds[key]))
                            {
                                //he is in the admin list so remove it
                                changes.push_back(std::make_pair("-o", cmds[key]));
                                remove_admin(channels[flag], cmds[key++]);
                            }
                        }
                        else
                        {
                            //he is not in the channel
                            buffer = "ircserver: error(401) " + client.nickName + " " + cmds[key] + " ::No such nick\n";
                            key++;
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                }
                buffer1.clear();
                for (int i = 0;i < changes.size();i++)
                {
                    buffer1 += changes[i].first;
                    if (!changes[i].second.empty())
                        x++;
                }
                if (x)
                    buffer1 += " ";
                for (int i = 0;i < changes.size();i++){
                    buffer1 += changes[i].second;
                    if (i+1 < changes.size())
                        buffer1 += " ";
                }
                buffer = "ircserver: " + channels[flag].channel_name + " " + client.nickName + " has changed mode:" + buffer1;
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
            else
            {
                //show the permissions on the channel
                buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
                return 0;
            }
        }
        else
        {
            buffer = "Error(403): " + cmds[1] + " :No such channel\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
            return 0;
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return (0);
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
                if (channels[flag].admin_list[i] == client.nickName)
                {
                    std::map<std::string, int>::iterator it = channels[flag].members.find(cmds[2]);
                    if (it != channels[flag].members.end())
                    {
                        buffer = "You have been kicked out of the channel\n";
                        send(it->second , buffer.c_str(), buffer.length(), 0);
                        for (std::map<std::string, int>::iterator ito = channels[flag].members.begin(); ito != channels[flag].members.end(); ito++) {
                            if (cmds.size() <= 3)
                                buffer = client.nickName + " has kicked " + cmds[2] + " (" + client.nickName + ")\n";
                            else
                                buffer = client.nickName + " has kicked " + cmds[2] + " (" + cmds[3] + ")\n";
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
    return 0;
}

//u need to finish this
int INVITE_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    int flag = -1;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        if (check_channel(fd, channels, cmds[2]) > 0)
        {
            flag = check_channel(fd, channels, cmds[2]);
            if (in_channel(channels[flag], cmds[1]))
            {
                buffer = "<ircserver> Error(443):" + cmds[3] + " " + channels[flag].channel_name + " is already on channel\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
            else
            {
                if (is_admin(channels[flag], client.nickName))
                {
                    channels[flag].invite_list.push_back(cmds[3]);
                    //tell me Inviting client2 to #ch1
                    //tell him tiko has invited you to #ch1
                    buffer = "Inviting " + cmds[1] + " to " + cmds[2] + "\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                    buffer = client.nickName + " has invited you to " + cmds[2] + "\n";
                    send(channels[flag].members[cmds[1]], buffer.c_str(), buffer.length(), 0);
                }
                else
                {
                    buffer = "ircserver Error(443): " + channels[flag].channel_name + " You're not channel operator\n";
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
    return 0;
}

int TOPIC_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    int flag = -1;
    std::string buffer;
    std::string topic;
    if (cmds.size() >= 2)
    {
        if (check_channel(fd, channels, cmds[1]) >= 0)
        {
            flag = check_channel(fd, channels, cmds[1]);
            if (channels[flag].is_topic)
            {
                if (is_admin(channels[flag], client.nickName))
                {
                    if (cmds.size() == 2)
                    {
                        buffer = "Topic: " + channels[flag].channel_topic + "\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                        if (channels[flag].topic_time)
                        {
                            buffer = channels[flag].who_set_topic + "set the topic at: " + myto_string(channels[flag].topic_time) + "\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if(cmds.size() >= 3)
                    {
                        for (int i = 2;i < cmds.size();i++)
                        {
                            topic += cmds[i];
                            if (i+1 < cmds.size())
                                topic += " ";
                        }
                        channels[flag].who_set_topic = client.nickName;
                        channels[flag].topic_time = time(0);
                        channels[flag].channel_topic = topic;
                        broad_cast(channels[flag], client.nickName, " has set topic: ", topic);
                    }
                }
                else
                {
                    buffer = "Error(482): " + channels[flag].channel_name + " :You're not channel operato\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
            }
            else
            {
                if (cmds.size() == 2)
                    {
                        buffer = "Topic: " + channels[flag].channel_topic + "\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                        if (channels[flag].topic_time)
                        {
                            buffer = channels[flag].who_set_topic + "set the topic at: " + myto_string(channels[flag].topic_time) + "\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if(cmds.size() >= 3)
                    {
                        for (int i = 2;i < cmds.size();i++)
                        {
                            topic += cmds[i];
                            if (i+1 < cmds.size())
                                topic += " ";
                        }
                        channels[flag].who_set_topic = client.nickName;
                        channels[flag].topic_time = time(0);
                        channels[flag].channel_topic = topic;
                        broad_cast(channels[flag], client.nickName, " has set topic: ", topic);
                    }
            }
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}

//need to handle sending to multiple clients a message the contain multiple words
int PRIVMSG_COMMAND(int fd, std::vector<std::string> &cmds, std::map<int, Client> &mapo, std::vector<Channels> &channels){
    int flag = 0;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        if (check_channel(fd, channels, cmds[1]) >= 0){
            //send it to all people in a channel because its a message to a channel
            flag = check_channel(fd, channels, cmds[1]);
            for (int j = 2;j < cmds.size();j++)
            {
                buffer += cmds[j];
                if (j+1 < cmds.size())
                    buffer += " ";
            }
            for (std::map<std::string, int>::iterator it =  channels[flag].members.begin();it != channels[flag].members.end();it++)
            {
                buffer = "ircserver: " + cmds[1] + " " + mapo[fd].nickName + " " + buffer + "\n";
                send(it->second, buffer.c_str(), buffer.length(), 0);
            }
        }
        else
        {
            for (std::map<int, Client>::iterator ito = mapo.begin(); ito != mapo.end(); ito++) {
                if (ito->second.nickName == cmds[1])
                {
                    flag = 1;
                    for (int j = 2;j < cmds.size();j++)
                    {
                        buffer += cmds[j];
                        if (j+1 < cmds.size())
                            buffer += " ";
                    }
                    buffer += "\n";
                    buffer = "ircserver: " + mapo[fd].nickName + " " + buffer + "\n";
                    send(ito->second.fd, buffer.c_str(), buffer.length(), 0);
                    break;
                }
            }
            if (!flag)
            {
                buffer = "ircserver error(401): " + cmds[2] + " No such nick\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}

void IS_COMMAND_VALID(int fd, std::string &str, std::map<int, Client> &mapo, std::vector<Channels> channels)
{
    std::vector<std::string> cmds;
    cmds = split(str, ' ');
    std::string cmd = cmds[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::string buffer;
    if (cmds.size() >= 2)
    {
        if (cmd == "JOIN")
            JOIN_COMMAND(fd, cmds, mapo[fd], channels);
        else if (cmd == "MODE")
            MODE_COMMAND(fd, cmds, mapo[fd], channels);
        else if (cmd == "KICK")
            KICK_COMMAND(fd, cmds, mapo[fd], channels);
        else if (cmd == "INVITE")
            INVITE_COMMAND(fd, cmds, mapo[fd], channels);
        else if (cmd == "TOPIC")
            TOPIC_COMMAND(fd, cmds, mapo[fd], channels);
        else if (cmd == "PRIVMSG")
            PRIVMSG_COMMAND(fd, cmds, mapo, channels);
        else
            buffer = "Error(421): " + cmds[0] + " Unknown command\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
    }
    else
    {
        buffer = "Error(461): " + cmds[0] + " :Not enough parameters" + "\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
}
