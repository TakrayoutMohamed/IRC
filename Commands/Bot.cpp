/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-jira <mel-jira@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 07:34:00 by mel-jira          #+#    #+#             */
/*   Updated: 2024/07/17 07:34:13 by mel-jira         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Commands.hpp"

std::string getRandomLine(const std::vector<std::string>& lines) {
    int index = rand() % lines.size();
    return lines[index];
}

void    HAKIM_COMMAND(int fd){
    std::string buffer;
    std::ifstream inputFile("info.txt");
    if (!inputFile){
        std::cerr << "Error opening file." << std::endl;
        return ;
    }
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();
    srand(static_cast<unsigned int>(time(0)));
    std::string randomLine = getRandomLine(lines);
    buffer = ":Bot el-hakim say: " + randomLine + "\r\n";
    send(fd, buffer.c_str(), buffer.length(), 0);
    std::cout << "Random Line: " << randomLine << std::endl;
}
