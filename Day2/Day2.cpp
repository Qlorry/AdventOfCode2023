// Day2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include "../Common/framework.h"

struct GemCounter {
    int red = 0;
    int green = 0;
    int blue = 0;

    bool operator >(const GemCounter& other) {
        if (red > other.red || green > other.green || blue > other.blue) 
        {
            return true;
        }
        return false;
    }

    void AddUpTo(const GemCounter& other)
    {
        if (red < other.red)
            red = other.red;
        if (green < other.green)
            green = other.green;
        if (blue < other.blue)
            blue = other.blue;
    }
};

GemCounter calc_current(std::string::const_iterator begin, std::string::const_iterator end)
{
    GemCounter current;

    std::regex index_r("\\d+");
    std::regex color_r("(?:red|green|blue)");
    std::smatch m;

    while (std::regex_search(begin, end, m, index_r))
    {
        begin = m.suffix().first;

        std::smatch color_m;
        std::regex_search(begin, end, color_m, color_r);
        if (color_m[0] == "red")
        {
            current.red += std::stoi(m[0]);
        }
        else if (color_m[0] == "green")
        {
            current.green += std::stoi(m[0]);
        }
        else if (color_m[0] == "blue")
        {
            current.blue += std::stoi(m[0]);
        }
        else {
            std::cout << "Panic!";
        }
    }

    return current;
}


void first_stage()
{
    PuzzleInput i("../Day2");

    GemCounter max{ 12, 13, 14 };
    std::regex index_r("\\d+");
    std::regex color_r("(?:red|green|blue)");

    int index_summ = 0;

    for (const auto& line : i.GetInputStrByLine())
    {
        std::smatch m;
        std::string::const_iterator searchStart(line.cbegin());
        std::regex_search(searchStart, line.cend(), m, index_r);
        int index = std::stoi(m[0]);
        searchStart = m.suffix().first;

        bool valid = true;

        for (auto it = std::find(searchStart, line.cend(), ';'); it != line.cend(); it = std::find(searchStart, line.cend(), ';'))
        {
            auto curr = calc_current(searchStart, it);
            if (curr > max) {
                valid = false;
                break;
            }
            searchStart = ++it;
        }

        if (valid)
        {
            //last
            auto curr = calc_current(searchStart, line.cend());
            valid = curr > max ? false : true;
        }

        if (valid)
        {
            index_summ += index;
        }
    }

    std::cout << "Result: " << index_summ;
}


void second_stage()
{
    PuzzleInput i("../Day2");

    std::regex index_r("\\d+");

    int power_summ = 0;

    for (const auto& line : i.GetInputStrByLine())
    {
        std::smatch m;
        std::string::const_iterator searchStart(line.cbegin());
        std::regex_search(searchStart, line.cend(), m, index_r);
        int index = std::stoi(m[0]);
        searchStart = m.suffix().first;

       
        GemCounter max;
        for (auto it = std::find(searchStart, line.cend(), ';'); it != line.cend(); it = std::find(searchStart, line.cend(), ';'))
        {
            max.AddUpTo(calc_current(searchStart, it));
            searchStart = ++it;
        }

        //last
        max.AddUpTo(calc_current(searchStart, line.cend()));
        power_summ += max.blue * max.green * max.red;
    }

    std::cout << "Result: " << power_summ;
}

int main()
{
    second_stage();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
