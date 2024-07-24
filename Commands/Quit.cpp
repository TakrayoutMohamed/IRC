/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/22 15:29:59 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/24 22:24:07 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

int QUIT_COMMAND(Client &client, std::vector<Channels> &channels){
    std::string buffer;
    std::string channel_to_leave;
    int flag = -1;
    for (size_t i = 0;i < client.inside_channel.size();i++){
        i = 0;
        flag = doesChannelExist(channels, client.inside_channel[i]);
        remove_member(channels[flag], client.nickName);
        channels[flag].members_count--;
        if (is_admin(channels[flag], client.inside_channel[i])){
            for (size_t j = 0;j < channels[flag].admin_list.size();j++){
                if (client.inside_channel[i] == channels[flag].admin_list[j]){
                    channels[flag].admin_list.erase(channels[flag].admin_list.begin() + j);
                    break ;
                }
            }
        }
        client.inside_channel.erase(client.inside_channel.begin()+i);
        buffer = ":" + client.nickName + "!" + client.userName + "@" + client.ip + " QUIT :EOF from client\r\n";
        broad_cast(channels[flag], buffer, "", "");
    }
    return 0;
}
