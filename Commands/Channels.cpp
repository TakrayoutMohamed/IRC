#include "Channels.hpp"

Channels::Channels()
{
    channel_name = "";
    channel_topic = "No topic is set.";
    channel_key = "";
    is_invite_only = false;
    is_limit = false;
    is_key = false;
    is_topic = false;
    topic = false;
    members_limit = 0;
    members_count = 0;
    topic_time = 0;
}
