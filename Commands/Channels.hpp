#pragma once

#include <string>
#include <vector>
#include <map>

class Channels
{
    public:
        std::string                 channel_name;
        bool                        is_invite_only;         //invite
        std::vector<std::string>    invite_list;
        bool                        is_limit;               //limit
        size_t                      members_limit;
        size_t                      members_count;
        bool                        is_topic;               //topic
        std::string                 channel_topic;
        bool                        topic;
        long long                   topic_time;
        std::string                 who_set_topic;
        bool                        is_key;                 //key
        std::string                 channel_key;
        std::vector<std::string>    admin_list;
        std::map<std::string, int>  members;
        long long                   channel_create_time;
        Channels();
};