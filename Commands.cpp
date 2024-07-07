/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:25:55 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/07 12:54:31 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

std::string myto_string(long long value) {
    std::stringstream oss;
    oss << value;
    return oss.str();
}

Channels::Channels()
{
    channel_name = "";
    channel_topic = "No topic is set.";
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
    for (size_t i = 0;i < changes.size();i++)
    {
        if (changes[i].first == look)
        {
            changes.erase(changes.begin()+i);
            break ;
        }
    }
}

bool already_op(Channels &channel, std::string &name){
    for (size_t i = 0;i < channel.admin_list.size();i++){
        if (channel.admin_list[i] == name)
            return (true);
    }
    return (false);
}

bool    in_channel(Channels &channel, std::string &name){
    std::string tmp = "@" + name;
    for (std::map<std::string, int>::iterator it = channel.members.begin();it != channel.members.end();it++){
        if (it->first == name || it->first == tmp)
            return (true);
    }
    return (false);
}

void    remove_admin(Channels &channel, std::string &name){
    for (size_t i = 0;i < channel.admin_list.size();i++){
        if (channel.admin_list[i] == name)
        {
            channel.admin_list.erase(channel.admin_list.begin()+i);
            break ;
        }
    }
}

bool    is_admin(Channels &channel, std::string &name){
    for (size_t i = 0;i < channel.admin_list.size();i++){
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

int check_channel(int fd, std::vector<Channels> &channels, std::string &channel_name, Client &client){
    int flag = -1;
    std::string buffer;
    printf("%s\n", channel_name.c_str());
    for (size_t i = 0;i < channels.size();i++)
    {
        if (channel_name == channels[i].channel_name)
        {
            flag = i;
            break ;
        }
    }
    if (flag == -1)
    {
        puts("we can't find the channel in the server");
        buffer = ":ircserver 403 " + client.nickName + " " + channel_name + " :No such channel\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return (flag);
}

void    broad_cast(Channels &channel, std::string error, std::string mid, std::string message){
    std::string buffer;
    for (std::map<std::string, int>::iterator it =  channel.members.begin();it != channel.members.end();it++)
    {
        buffer = error + mid + message;
        printf("the message >%s\n", buffer.c_str());
        send(it->second, buffer.c_str(), buffer.length(), 0);
    }
}

bool imInChannel(Channels &channel, std::string &name){
    for (size_t i = 0;i < channel.admin_list.size();i++){
        if (name == channel.admin_list[i]){
            return (true);
        }
    }
    if (channel.members.find(name) != channel.members.end()){
        return (true);
    }
    return (false);
}

void    create_join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client){
    puts("create and join channel");
    Channels channel;
    std::string buffer;
    channel.channel_name = command.channels_name[index];
    channel.channel_create_time = time(0);
    channel.members.insert(std::make_pair("@" + client.nickName, client.fd));
    channel.admin_list.push_back(client.nickName);
    channel.channel_topic = "No topic is set.";
    client.inside_channel.push_back(std::make_pair(command.channels_name[index], true));
    channels.push_back(channel);
    buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " JOIN " + command.channels_name[index] + "\r\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 353 " + client.nickName + " @ " + command.channels_name[index] + " :@" + client.nickName + "\r\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
    buffer = ":IRCSERVER 366 " + client.nickName + " " + command.channels_name[index] + " :End of /NAMES list.\r\n";
    send(client.fd, buffer.c_str(), buffer.length(), 0);
}

void    join_channel(std::vector<Channels> &channels, p_c &command, int index, Client &client)
{
    puts("join channel only");
    std::string buffer;
    channels[index].members.insert(std::make_pair(client.nickName, client.fd));
    if (channels[index].is_invite_only)
    {
        for (size_t i = 0;i < channels[index].invite_list.size();i++)
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
        buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " JOIN " + command.channels_name[index] + "\r\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 353 " + client.nickName + " @ " + command.channels_name[index] + " :";
        for (std::map<std::string, int>::iterator ito = channels[index].members.begin(); ito != channels[index].members.end(); ito++)
        {
            buffer += ito->first;
            std::map<std::string, int>::iterator nextIto = ito;
            ++nextIto;
            if (nextIto != channels[0].members.end()) {
                buffer += " ";
            }
        }
        buffer += "\r\n";
        send(it->second, buffer.c_str(), buffer.length(), 0);
        buffer = ":IRCSERVER 366 " + client.nickName + " " + command.channels_name[index] + " :End of /NAMES list.\r\n";
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
    if (cmds.size() >= 2) //check the size if there is something after join
    {
        for (size_t i = 0; i < command.channels_name.size();i++) //iterate on the channel names
        {
            flag = -1;
            if (command.channels_name[i][0] == '#' && command.channels_name[i].size() > 1 && !strchr(" \a\0\n\r,", command.channels_name[i][1])) // check if this channel is valid first
            {
                for (size_t j = 0; j < channels.size();j++) //try to find the channel
                {
                    if (command.channels_name[i] == channels[j].channel_name)
                    {
                        flag = j;
                        break ;
                    }
                }
                if (flag == -1){ // the channel doesn't exist create it and join it
                    create_join_channel(channels, command, i, client);
                }
                else
                {
                    puts("channel exist");
                    // most important check if u already in the channel
                    if (channels[flag].members.find(client.nickName) != channels[flag].members.end()){
                        puts("user already in channel");
                        return 0; // if we return that's because our user is in the channel and we should do nothing
                    }
                    if (channels[flag].is_invite_only) // check if the channel is invite only
                    {
                        for (size_t j = 0; j < channels[flag].invite_list.size();j++) // check if the user who apply the command to join is on the invite list
                        {
                            if (client.nickName == channels[flag].invite_list[j]) // if this true this mean we are in the invite list
                            {
                                if (channels[flag].is_limit && channels[flag].members_limit >= channels[flag].members.size()) // this may be wrong i will come back to it later
                                {
                                    //we exceed the limit fo the member in the channels
                                    buffer = ":ircserver Error(743): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+l)\r\n";
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
                                            buffer = ":ircserver Error(475): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+k)\r\n";
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
                                buffer = ":ircserver Error(473): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+i)\r\n";
                                send(fd, buffer.c_str(), buffer.length(), 0);
                                break ;
                            }
                        }
                    }
                    else
                    {
                        //check limit
                        puts("the channel is not invite only");
                        if (channels[flag].is_limit && channels[flag].members_limit <= channels[flag].members.size())
                        {
                            printf("members size =[%lu] and the limit is=[%lu]\n", channels[flag].members.size(), channels[flag].members_limit);
                            puts("max limit");
                            puts("i will print the members in the channel");
                            for (std::map<std::string, int>::iterator iton = channels[flag].members.begin(); iton != channels[flag].members.end();iton++){
                                printf("%s\n", iton->first.c_str());
                            }
                            //we exceed the limit of the member in the channels
                            buffer = ":ircserver Error(471): " + client.nickName + " " + channels[flag].channel_name + " :Cannot join channel (+l)\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].channel_key.empty()) // channel has a key
                        {
                            puts("channel need key");
                            if (cmds.size() >= 3)
                            {
                                puts("the user entered a key");
                                if (channels[flag].channel_key == command.channels_key[p])
                                {
                                    puts("the user key is correct");
                                    //key is correct join the channel join the channel
                                    join_channel(channels, command, flag, client);
                                }
                                else
                                {
                                    puts("the user key is uncorrect");
                                    //  "<client> <channel> :Cannot join channel (+k)"
                                    buffer = ":ircserver Error(475): " + channels[flag].channel_name + " :Cannot join channel (+k)\r\n";
                                    send(fd, buffer.c_str(), buffer.length(), 0);
                                }
                            }
                        }
                        else
                        {
                            puts("the channel has no limit or key");
                            // join the channel because it doesn't have a key
                            join_channel(channels, command, flag, client);
                        }
                    }
                }
            }
            else
            {
                buffer = ":ircserver Error(403): " + cmds[0] + " :No such channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    return 0;
}

int     MODE_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    // for (size_t i = 0;i < cmds.size();i++)
    // {
    //     std::cout << cmds[i] << " ";
    // }
    // std::cout << std::endl;
    size_t arg = 0;
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
        for (size_t j = 0; j < channels.size();j++) //try to find the channel
        {
            if (cmds[1] == channels[j].channel_name){
                flag = j;
                break ;
            }
        }
        if (flag != -1) //if true the channel does exist
        {
            if (cmds.size() >= 3) // if this false it mean its mode with a valid channel name but nothing next so there should be some default output
            {
                for (int i = 0;cmds[2][i];i++){ // iterate on the modes 
                    while (cmds[2][i] && (cmds[2][i] == '+' || cmds[2][i] == '-')){ // find the sign
                        sign = cmds[2][i]; // store the sign
                        i++;
                    }
                    if (sign == '+' && cmds[2][i])
                        buffer1 += sign;
                    else if (sign == '-' && cmds[2][i])
                        buffer1 += sign;
                    if (strchr("lkito", cmds[2][i]))
                        buffer1 += cmds[2][i];
                    else {
                        error = cmds[2][i];
                        buffer1 = ":ircserver 472 " + client.nickName + " " + error + " :is an unknown mode char to me\r\n";
                        send(fd, buffer1.c_str(), buffer1.length(), 0);
                        return 0;
                    }
                }
                for (int i = 0; buffer1[i]; i += 2){
                    if (buffer1[i] && buffer1[i+1] && buffer1[i] == '+' && strchr("kl", buffer1[i+1])){
                        // puts("increase the number of required arguments");
                        arg++;
                    }
                    else if (buffer1[i] && buffer1[i+1] && buffer1[i] == '-' && strchr("k", buffer1[i+1])){
                        // puts("increase the number of required arguments");
                        arg++;
                    }
                }
                // printf("size-2=[%lu] required-arg=[%lu]\n", cmds.size(), arg);
                if (arg > cmds.size() - 3){
                    buffer = ":ircserver 461: " + cmds[0] + " :Not enough parameters\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                    return 0;
                }
                for (size_t i = 0;i < buffer1.length();i +=2)
                {
                    if (!strncmp(&buffer1[i], "+i", 2)){
                        if (channels[flag].is_invite_only){
                            buffer = ":channel is already set to invite only: on\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_invite_only){
                            lookfor("-i", changes);
                            changes.push_back(std::make_pair("+i", ""));
                            channels[flag].is_invite_only = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-i", 2)){
                        if (channels[flag].is_invite_only){
                            lookfor("+i", changes);
                            changes.push_back(std::make_pair("-i", ""));
                            channels[flag].is_invite_only = false;
                        }
                        else if (!channels[flag].is_invite_only){
                            buffer = ":channel is already set to invite only: off\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+t", 2)){
                        if (channels[flag].is_topic){
                            buffer = ":channel is already set to topic: on\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_topic){
                            lookfor("-t", changes);
                            changes.push_back(std::make_pair("+t", ""));
                            channels[flag].is_topic = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-t", 2)){
                        if (channels[flag].is_topic){
                            lookfor("+t", changes);
                            changes.push_back(std::make_pair("-t", ""));
                            channels[flag].is_topic = false;
                            channels[flag].channel_topic = "";
                        }
                        else if (!channels[flag].is_topic){
                            buffer = ":channel is already set to topic: off\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+k", 2)){
                        if (channels[flag].is_key){
                            buffer = ":ircserver 467 " + client.nickName + " " + channels[flag].channel_name + " ::Channel key already set\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                        else if (!channels[flag].is_key){
                            lookfor("-k", changes);
                            changes.push_back(std::make_pair("+k", cmds[key]));
                            channels[flag].channel_key = cmds[key++];
                            channels[flag].is_key = true;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-k", 2)){
                        if (channels[flag].is_key){
                            if (channels[flag].channel_key == cmds[key]){
                                lookfor("+k", changes);
                                changes.push_back(std::make_pair("-k", ""));
                                channels[flag].is_key = false;
                                channels[flag].channel_key = "";
                                key++;
                            }
                            else{
                                buffer = ":ircserver 467 " + client.nickName + " " + channels[flag].channel_name + " :Channel key already set\r\n";
                                send(fd, buffer.c_str(), buffer.length(), 0);
                            }
                        }
                        else if (!channels[flag].is_key){
                            buffer = ":ircserver 467 " + client.nickName + " " + channels[flag].channel_name + " :Channel key already set\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+l", 2)){
                        if (channels[flag].is_limit){
                            if ((int)channels[flag].members_limit != atoi(cmds[key].c_str()) && atoi(cmds[key].c_str()) > 0){
                                lookfor("+l", changes);
                                changes.push_back(std::make_pair("+l", cmds[key].c_str()));
                                channels[flag].members_limit = atoi(cmds[key].c_str());
                                key++;
                            }
                        }
                        else if (!channels[flag].is_limit){
                            lookfor("-l", changes);
                            changes.push_back(std::make_pair("+l", cmds[key].c_str()));
                            channels[flag].members_limit = atoi(cmds[key].c_str());
                            channels[flag].is_limit = true;
                            key++;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-l", 2)){
                        if (channels[flag].is_limit){
                            lookfor("+l", changes);
                            changes.push_back(std::make_pair("-l", ""));
                            channels[flag].is_limit = false;
                        }
                    }
                    else if (!strncmp(&buffer1[i], "+o", 2)){
                        puts("add a moderator");
                        //check if is he in the channel
                        if (in_channel(channels[flag], cmds[key]))
                        {   
                            if (already_op(channels[flag], cmds[key])){
                                puts("the user already admin");
                                //already admin ignore it
                                key++;
                            }
                            else{
                                puts("add the user to admin list");
                                //add him to the admin list
                                changes.push_back(std::make_pair("+o", cmds[key]));
                                channels[flag].admin_list.push_back(cmds[key++]);
                            }
                        }
                        else{
                            puts("user is not inside the channel");
                            //he is not in the channel
                            buffer = ":ircserver 401 " + client.nickName + " " + cmds[key] + " :No such nick\r\n";
                            key++;
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if (!strncmp(&buffer1[i], "-o", 2)){
                        puts("remove a moderator");
                        if (in_channel(channels[flag], cmds[key])){
                            if (!already_op(channels[flag], cmds[key])){
                                puts("the user is not admin");
                                //he is not an op so do nothing
                                key++;
                            }
                            else if (already_op(channels[flag], cmds[key])){
                                //he is in the admin list so remove it
                                puts("remove the user from admin list");
                                changes.push_back(std::make_pair("-o", cmds[key]));
                                remove_admin(channels[flag], cmds[key]);
                                key++;
                            }
                        }
                        else{
                            puts("user is not inside the channel hhh");
                            //he is not in the channel
                            buffer = ":ircserver 401 " + client.nickName + " " + cmds[key] + " :No such nick\r\n";
                            std::cout << buffer << std::endl;
                            key++;
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                }
                buffer1.clear();
                for (size_t i = 0;i < changes.size();i++){
                    buffer1 += changes[i].first;
                    if (!changes[i].second.empty())
                        x++;
                }
                if (x)
                    buffer1 += " ";
                for (size_t i = 0;i < changes.size();i++){
                    buffer1 += changes[i].second;
                    if (i+1 < changes.size())
                        buffer1 += " ";
                }
                buffer = ":" + client.nickName + "!" + client.userName + client.ip + " MODE " + channels[flag].channel_name + " " + buffer1 + "\r\n";
                std::cout << "buffer >> " << buffer; 
                broad_cast(channels[flag], buffer, "", "");
                // send(fd, buffer.c_str(), buffer.length(), 0);
            }
            else{
                //show the permissions on the channel
                buffer = ":ircserver 461: " + cmds[0] + " :Not enough parameters\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
                return 0;
            }
        }
        else{
            buffer = ":ircserver 403: " + cmds[1] + " :No such channel\r\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
            return 0;
        }
    }
    else{
        buffer = ":ircserver 461: " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return (0);
}

//kick is done
int KICK_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels)
{
    int kicked = 0;
    int flag = -1;
    std::string buffer;
    if (cmds.size() >= 3)
    {
        for (size_t i = 0;i < channels.size();i++)
        {
            if (cmds[1] == channels[i].channel_name)
            {
                flag = i;
                break ;
            }
        }
        if (flag == -1)
        {
            //no such channel
            buffer = ":ircserver Error(403): " + cmds[1] + " :No such channel\r\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
        else
        {
            //the channel exist
            for (size_t i = 0;i< client.inside_channel.size();i++)
            {
                if (channels[flag].admin_list[i] == client.nickName)
                {
                    for (std::map<std::string, int>::iterator it = channels[flag].members.begin();it != channels[flag].members.end();it++)
                    {
                        if (it->first == cmds[2] || it->first == ("@" + cmds[2])){
                            // :reda1!~a@197.230.30.146 KICK #ch2 reda3 :reda3
                            puts("u find the member and u kicked him");
                            if (cmds.size() <= 3 || cmds[3] == ":")
                                buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " KICK " + cmds[1] + " " + cmds[2] + " :" + cmds[2] + "\r\n";
                            else
                                buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " KICK " + cmds[1] + " " + cmds[2] + " :" + cmds[3] + "\r\n";
                            broad_cast(channels[flag], buffer, "", "");
                            channels[flag].members.erase(it);
                            // a small note the client know what channels he have entered but we never delete them when he left them if it cause a problem
                            //fix it by removing channels_inside or just look for them and delete them xD
                            kicked = 1;
                            break ;
                        }
                    }
                    for (size_t j = 0;j < channels[flag].admin_list.size();j++)
                    {
                        if (cmds[2] == channels[flag].admin_list[j]){
                            puts("u find the member and u kicked him again in the admin list");
                            channels[flag].admin_list[j].erase();
                            kicked = 1;
                            break ;
                        }
                    }
                    if (!kicked){
                        puts("u couldn't find the member u wanna kick");
                        buffer = ":ircserver 441 " + cmds[2] + " " + channels[flag].channel_name + " They aren't on that channel\r\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                    }
                }
                if (i+1 == client.inside_channel.size())
                {
                    buffer = ":ircserver 482 " + client.nickName + " " + channels[flag].channel_name + " :You're not channel operato\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
            }
        }
    }
    else
    {
        buffer = ":ircserver Error(461): " + cmds[0] + " :Not enough parameters\r\n";
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
        if (check_channel(fd, channels, cmds[2], client) > 0)
        {
            flag = check_channel(fd, channels, cmds[2], client);
            if (in_channel(channels[flag], cmds[1]))
            {
                buffer = ":ircserver Error(443):" + cmds[3] + " " + channels[flag].channel_name + " is already on channel\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
            else
            {
                if (is_admin(channels[flag], client.nickName))
                {
                    channels[flag].invite_list.push_back(cmds[3]);
                    //tell me Inviting client2 to #ch1
                    //tell him tiko has invited you to #ch1
                    buffer = "Inviting " + cmds[1] + " to " + cmds[2] + "\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                    buffer = client.nickName + " has invited you to " + cmds[2] + "\r\n";
                    send(channels[flag].members[cmds[1]], buffer.c_str(), buffer.length(), 0);
                }
                else
                {
                    buffer = ":ircserver Error(443): " + channels[flag].channel_name + " You're not channel operator\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
            }
        }
    }
    else
    {
        buffer = ":ircserver Error(461): " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}

//topic +ns
int TOPIC_COMMAND(int fd, std::vector<std::string> &cmds, Client &client, std::vector<Channels> &channels){
    // for (size_t i = 0;i< cmds.size();i++){
    //     std::cout << cmds[i] + " ";
    // }
    // std::cout << std::endl;
    int flag = -1;
    std::string buffer;
    std::string topic;
    if (cmds.size() >= 2)
    {
        if (check_channel(fd, channels, cmds[1], client) >= 0)
        {
            puts("the channel does exist");
            flag = check_channel(fd, channels, cmds[1], client);
            if (channels[flag].is_topic) // if it true only mod can set topic
            {
                puts("the topic is set to true");
                if (is_admin(channels[flag], client.nickName))
                {
                    puts("u are an admin u have the permission");
                    if (cmds.size() == 2)
                    {
                        puts("just display the current topic set");
                        buffer = "Topic: " + channels[flag].channel_topic + "\r\n";
                        send(fd, buffer.c_str(), buffer.length(), 0);
                        if (channels[flag].topic_time)
                        {
                            buffer = channels[flag].who_set_topic + "set the topic at: " + myto_string(channels[flag].topic_time) + "\r\n";
                            send(fd, buffer.c_str(), buffer.length(), 0);
                        }
                    }
                    else if(cmds.size() >= 3)
                    {
                        puts("set a new topic");
                        for (size_t i = 2;i < cmds.size();i++)
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
                    puts("u don't have the permission");
                    buffer = "Error(482): " + channels[flag].channel_name + " :You're not channel operato\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
            }
            else //anyone can set the topic
            {
                puts("the channel doesn't have a topic");
                if (cmds.size() == 2){
                    puts("here 1");
                    buffer = ":ircserver 331 " + client.nickName + " " + channels[flag].channel_name + " :" + channels[flag].channel_topic + "\r\n";
                    send(fd, buffer.c_str(), buffer.length(), 0);
                }
                else if(cmds.size() >= 3)
                {
                    puts("here 2");
                    for (size_t i = 2;i < cmds.size();i++)
                    {
                        topic += cmds[i];
                        if (i+1 < cmds.size())
                            topic += " ";
                    }
                    topic += "\r\n";
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
        buffer = ":ircserver Error(461): " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}

//need to handle sending to multiple clients a message the contain multiple words
int PRIVMSG_COMMAND(int fd, std::vector<std::string> &cmds, std::map<int, Client> &mapo, std::vector<Channels> &channels){
    int flag = 0;
    std::string buffer;
    std::string buffer1;
    if (cmds.size() >= 3)
    {
        if (cmds[1][0] == '#' && check_channel(fd, channels, cmds[1], mapo[fd]) >= 0){
            //the channel does exist
            puts("this message is to a channel");
            flag = check_channel(fd, channels, cmds[1], mapo[fd]);
            if (imInChannel(channels[flag], mapo[fd].nickName)){ // check if u are in the channel if yes then u can send the message in the channel
                puts("im in the channel");
                for (size_t j = 2;j < cmds.size();j++)
                {
                    buffer1 += cmds[j];
                    if (j+1 < cmds.size())
                        buffer1 += " ";
                }
                // printf("size=[%lu] this is my message:%s\n",channels[flag].members.size(), buffer.c_str());
                for (std::map<std::string, int>::iterator it =  channels[flag].members.begin();it != channels[flag].members.end();it++)
                {
                    puts("send message");
                    if (it->second == fd)
                        continue ;
                    buffer = ":" + mapo[fd].nickName + "!~" + mapo[fd].userName + mapo[fd].ip + " PRIVMSG " + cmds[1] + " "+ buffer1 + "\r\n";
                    send(it->second, buffer.c_str(), buffer.length(), 0);
                }
            }
            else{
                puts("im not in the channel");
                buffer = ":ircserver: " + mapo[fd].nickName + " " + channels[flag].channel_name + " :You're not on that channel" + "\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
        else //the message is to a user
        {
            if (cmds[1][0] == '#' )
                return 0;
            for (std::map<int, Client>::iterator ito = mapo.begin(); ito != mapo.end(); ito++) { //search for the user in the server members
                if (ito->second.nickName == cmds[1]) //if u find him send him the message
                {
                    flag = 1;
                    for (size_t j = 2;j < cmds.size();j++)
                    {
                        buffer += cmds[j];
                        if (j+1 < cmds.size())
                            buffer += " ";
                    }
                    buffer = ":" + mapo[fd].nickName + "!" + mapo[fd].userName + "@" + mapo[fd].ip + " PRIVMSG " + cmds[1] + " " + buffer + "\r\n";
                    send(ito->second.fd, buffer.c_str(), buffer.length(), 0);
                    break;
                }
            }
            if (!flag) //if u didn't find him send this error
            {
                buffer = ":ircserver error(401): " + cmds[2] + " No such nick\r\n";
                send(fd, buffer.c_str(), buffer.length(), 0);
            }
        }
    }
    else
    {
        buffer = ":ircserver Error(461): " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
    return 0;
}

void IS_COMMAND_VALID(int fd, std::string &str, std::map<int, Client> &mapo, std::vector<Channels> &channels)
{
    std::cout << str << std::endl;
    std::vector<std::string> cmds;
    cmds = split(str, ' ');
    std::string cmd = cmds[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::string buffer;
    if (cmds.size() >= 2)
    {
        if (cmd == "JOIN")
        {
            JOIN_COMMAND(fd, cmds, mapo[fd], channels);
        }
        else if (cmd == "MODE"){
            MODE_COMMAND(fd, cmds, mapo[fd], channels);
        }
        else if (cmd == "KICK"){
            KICK_COMMAND(fd, cmds, mapo[fd], channels);
        }
        else if (cmd == "INVITE"){
            INVITE_COMMAND(fd, cmds, mapo[fd], channels);
        }
        else if (cmd == "TOPIC"){
            TOPIC_COMMAND(fd, cmds, mapo[fd], channels);
        }
        else if (cmd == "PRIVMSG"){
            PRIVMSG_COMMAND(fd, cmds, mapo, channels);
        }
        else
        {
            buffer = ":ircserver Error(421): " + cmds[0] + " Unknown command\r\n";
            send(fd, buffer.c_str(), buffer.length(), 0);
        }
    }
    else
    {
        buffer = ":ircserver Error(461): " + cmds[0] + " :Not enough parameters\r\n";
        send(fd, buffer.c_str(), buffer.length(), 0);
    }
}
