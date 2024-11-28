// Day1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include "../Common/framework.h"


void first_stage()
{
    PuzzleInput i("../Day1");

    std::regex r("[0-9]");
    int summ = 0;
    for (const auto& line : i.GetInputStrByLine())
    {
        char res[2];

        std::smatch m;
        std::regex_search(line.begin(), line.end(), m, r);
        res[0] = *(line.begin() + m.position());

        std::match_results<std::string::const_reverse_iterator> m2;

        std::regex_search(line.rbegin(), line.rend(), m2, r);
        res[1] = *(line.rbegin() + m2.position());

        summ += std::stoi(res);
    }


    std::cout << "Result: " << summ;
}

int parse_num(const std::string& str)
{
    if (std::isdigit(str[0]))
    {
        return std::stoi(str);
    }
    
    if (str[0] == 't')
    {
        if (str == "two")
        {
            return 2;
        }
        else if (str == "three")
        {
            return 3;
        }
        else
        {
            return -1;
        }
    }
    else if (str[0] == 'f')
    {
        if (str == "four")
        {
            return 4;
        }
        else if (str == "five")
        {
            return 5;
        }
        else
        {
            return -1;
        }
    }
    else if (str[0] == 's')
    {
        if (str == "six")
        {
            return 6;
        }
        else if (str == "seven")
        {
            return 7;
        }
        else
        {
            return -1;
        }
    }
    else if (str == "one")
    {
        return 1;
    }
    else if (str == "eight")
    {
        return 8;
    }
    else if (str == "nine")
    {
        return 9;
    }
    else 
    {
        return -1;
    }
}

void second_stage()
{
    PuzzleInput i("../Day1");

    std::regex r("(?:[1-9]|one|two|three|four|five|six|seven|eight|nine)");
    int summ = 0;
    int ind = 0;
    for (const auto& line : i.GetInputStrByLine())
    {
        int res_num = 0;

        std::smatch m;
        std::regex_search(line, m, r);
        res_num += 10 * parse_num(m[0]);

        std::string tmp_line = line;
        auto last_line = tmp_line;
        for (std::smatch sm; std::regex_search(tmp_line, sm, r);)
        {
            last_line = sm[0];
            tmp_line = tmp_line.substr( sm.position() + 1);
        }

        res_num += parse_num(last_line);

        summ += res_num;
    }

    std::cout << "Result: " << summ;
}

int main(int argc, const char** argv)
{
    
    second_stage();

}
