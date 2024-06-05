/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:25:53 by mel-jira          #+#    #+#             */
/*   Updated: 2024/06/05 13:13:49 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <ctime>
#include <sys/socket.h>

#define ERR_UNKNOWNCOMMAND 421
#define ERR_NEEDMOREPARAMS 461
#define ERR_NOSUCHCHANNEL 403
#define ERR_TOOMANYCHANNELS 405
#define ERR_BADCHANNELKEY 475
#define ERR_BANNEDFROMCHAN 474
#define ERR_CHANNELISFULL 471
#define ERR_INVITEONLYCHAN 473
#define ERR_BADCHANMASK 476
#define RPL_TOPIC 332
#define RPL_TOPICWHOTIME 333
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366

typedef struct parsing_command
{
    std::string command;
    std::vector<std::string> channels_name;
    std::vector<std::string> channels_key;
}   p_c;

class Client
{
    
    public:
        std::string nickname;
        std::string ip;
        int fd;
        std::vector<std::pair<std::string, bool>> inside_channel;
        Client();
};

class Channels
{
    public:
        std::string                 channel_name;
        bool                        is_invite_only;         //invite
        std::vector<std::string>    invite_list;
        bool                        is_limit;               //limit
        int                         members_limit;
        bool                        is_topic;               //topic
        std::string                 channel_topic;
        long long                   topic_time;
        std::string                 who_set_topic;
        bool                        is_key;                 //key
        std::string                 channel_key;
        std::vector<std::string>    admin_list;
        std::map<std::string, int>  members;
        long long                   channel_create_time;
        Channels();
};

void IS_COMMAND_VALID(int fd, std::string &str, std::map<int, Client> &mapo, std::vector<Channels> channels);
