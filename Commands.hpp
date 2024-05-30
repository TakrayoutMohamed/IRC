/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:25:53 by mel-jira          #+#    #+#             */
/*   Updated: 2024/05/30 18:14:38 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
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

typedef struct s_mode
{
    std::string l;
    int ml;
    int pl;
    std::string k;
    int mk;
    int pk;
    std::string o;
    int mo;
    int po;
    std::string i;
    int mi;
    int pi;
    std::string t;
    int mt;
    int pt;
}   Mode;

class Client
{
    public:
        std::string nickname;
        std::string password;
        std::string ip;
        int fd;
        std::vector<std::pair<std::string, bool>> inside_channel;
        Client();
};

class Channels
{
    public:
        std::string channel_name;
        std::string channel_topic;
        std::string channel_key;
        bool        is_invite_only;
        std::vector<std::string> invite_list;
        std::vector<std::string> admin_list;
        bool is_limit;
        int members_limit;
        std::map<std::string, int> members;
        long long channel_create_time;
        Channels();
};

int IS_COMMAND_VALID(int fd, std::string &str, std::map<int, Client> &mapo, std::vector<Channels> channels);
