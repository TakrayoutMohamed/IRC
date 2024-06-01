#include <iostream>
#include <cstring>
#include <string>

int main()
{
    std::string input = "-+-+-+-+++--l+-+++---++i-+-+++----+kb";
    std::string buffer1;
    
    int arg = 0;
    int i = 0;
    char sign = '+';
    int space = 0;
    while (input[i])
    {
        while (input[i] && (input[i] == '+' || input[i] == '-'))
        {
            sign = input[i];
            i++;
        }
        if (arg == 0 && sign == '+')
            buffer1 += sign;
        else if (sign == '-' && arg > 0 && space == 0)
            buffer1 += " " + sign;
        else if (space == 0 && arg == 0)
            buffer1 += sign;
        if (strchr("lkito", input[i]))
            buffer1 += input[i];
        std::cout << input[i] << std::endl;
        std::cout << buffer1 << std::endl;
        i++;
    }

    return 0;
}