#include <iostream>
#include <cstring>
#include <string>

typedef struct mode
{
    int i;
    int l;
    int o;
    int k;
    int t;
} mode;

void    help_init(mode *help)
{
    help->i = 0;
    help->l = 0;
    help->o = 0;
    help->k = 0;
    help->t = 0;
}

int main()
{
    std::string input = "+iiiiii+++iiii+++tttt----tttt-iiii+illl-lll-ooo-o+ooo+kkkkkk-kkkkk+k-iii-ttt-itititlolll-kkk-kokokok--------iiittt";
    //          rr       +i+t-t-i+il-l-ooo-o+ooo+k-k+k-i-o-k-ooo-t    
    //                   +it-ti+illoooo+oook-k+k-iokooo
    // i == -1 , l == -1 , t == -1 , k == -1 
    std::string buffer1;
    std::string buffer;

    int arg = 0;
    char sign = '+';
    std::string error = "i";
    mode help;
    help_init(&help);
    for (int i = 0;input[i];i++)
    {
        while (input[i] && (input[i] == '+' || input[i] == '-'))
        {
            sign = input[i];
            i++;
        }
        if (sign == '+' && input[i])
            buffer1 += sign;
        else if (sign == '-' && input[i])
            buffer1 += sign;
        if (strchr("lkito", input[i]))
            buffer1 += input[i];
        else
        {
            error = input[i];
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
    //before u continue check if the number of argument is ok if anything wrong give the error and return
    for (int i = 0; buffer1[i]; i += 2)
    {
        if (!strncmp(&buffer1[i], "+i", 2))
        {
            int position = buffer.find("-i");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.i != 1)
                buffer += "+i";
            help.i = 1;
        }
        else if (!strncmp(&buffer1[i], "+l", 2)){
            int position = buffer.find("-l");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.l != 1)
                buffer += "+l";
            help.l = 1;
        }
        else if (!strncmp(&buffer1[i], "+k", 2)){
            int position = buffer.find("-k");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.k != 1)
                buffer += "+k";
            help.k = 1;
        }
        else if (!strncmp(&buffer1[i], "+t", 2)){
            int position = buffer.find("-t");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.t != 1)
                buffer += "+t";
            help.t = 1;
        }
        else if (!strncmp(&buffer1[i], "+o", 2)){
            buffer += "+o";
        }
        else if (!strncmp(&buffer1[i], "-i", 2)){
            int position = buffer.find("+i");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.i != -1)
                buffer += "-i";
            help.i = -1;
        }
        else if (!strncmp(&buffer1[i], "-l", 2)){
            int position = buffer.find("+l");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.l != -1)
                buffer += "-l";
            help.l = -1;
        }
        else if (!strncmp(&buffer1[i], "-k", 2)){
            int position = buffer.find("+k");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.k != -1)
                buffer += "-k";
            help.k = -1;
        }
        else if (!strncmp(&buffer1[i], "-t", 2)){
            int position = buffer.find("+t");
            if (position != -1)
                buffer = buffer.erase(position, 2);
            if (help.t != -1)
                buffer += "-t";
            help.t = -1;
        }
        else if (!strncmp(&buffer1[i], "-o", 2)){
            buffer += "-o";
        }
    }
    
    std::cout << "buffer1:" << buffer1 << std::endl;
    std::cout << "buffer:" << buffer << std::endl;
    std::cout << "number of arg=" << arg << std::endl;
    return 0;
}
