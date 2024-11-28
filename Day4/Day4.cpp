// Day4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include <numeric>
#include <map>
#include "../Common/framework.h"

void first_stage()
{
    PuzzleInput i("../Day4");

    std::regex index_r("\\d+");

    int power_summ = 0;
    std::vector<int> win_nums;
    std::vector<int> my_nums;

    for (const auto& line : i.GetInputStrByLine())
    {
        win_nums.clear();
        my_nums.clear();

        std::smatch m;
        std::string::const_iterator searchStart(line.cbegin());
        std::regex_search(searchStart, line.cend(), m, index_r);
        int index = std::stoi(m[0]);
        searchStart = m.suffix().first;


        auto separator = std::find(searchStart, line.cend(), '|');
        if (separator == line.cend())
        {
            std::puts("PANIC!");
        }

        while (std::regex_search(searchStart, separator, m, index_r))
        {
            win_nums.push_back(std::stoi(m[0]));
            searchStart = m.suffix().first;
        }

        searchStart = separator;

        while (std::regex_search(searchStart, line.cend(), m, index_r))
        {
            my_nums.push_back(std::stoi(m[0]));
            searchStart = m.suffix().first;
        }

        std::vector<int>intersection;
        std::sort(win_nums.begin(), win_nums.end());
        std::sort(my_nums.begin(), my_nums.end());
        std::set_intersection(win_nums.begin(), win_nums.end(),
            my_nums.begin(), my_nums.end(),
            std::back_inserter(intersection));
        if (!intersection.empty())
        {
            power_summ += std::pow(2, intersection.size() - 1);
        }
    }

    std::cout << "Result: " << power_summ;
}

std::vector<int> process_card(const std::string& line)
{
    std::vector<int> win_nums;
    std::vector<int> my_nums;
    std::regex index_r("\\d+");

    std::smatch m;
    std::string::const_iterator searchStart(line.cbegin());
    searchStart = std::find(searchStart, line.cend(), ':');

    auto separator = std::find(searchStart, line.cend(), '|');
    if (separator == line.cend())
    {
        std::puts("PANIC!");
    }

    while (std::regex_search(searchStart, separator, m, index_r))
    {
        win_nums.push_back(std::stoi(m[0]));
        searchStart = m.suffix().first;
    }

    searchStart = separator;

    while (std::regex_search(searchStart, line.cend(), m, index_r))
    {
        my_nums.push_back(std::stoi(m[0]));
        searchStart = m.suffix().first;
    }

    std::vector<int>intersection;
    std::sort(win_nums.begin(), win_nums.end());
    std::sort(my_nums.begin(), my_nums.end());
    std::set_intersection(win_nums.begin(), win_nums.end(),
        my_nums.begin(), my_nums.end(),
        std::back_inserter(intersection));

    return intersection;
}

void second_stage()
{
    PuzzleInput inp("../Day4");

    std::regex index_r("\\d+");

    std::map<int, int> cpy_cnt;
    for (auto i = 1; i <= inp.GetInputStrByLine().size(); i++)
    {
        cpy_cnt[i] = 1;
    }
    
    for (const auto& line : inp.GetInputStrByLine())
    {
        std::smatch m;
        std::string::const_iterator searchStart(line.cbegin());
        std::regex_search(searchStart, line.cend(), m, index_r);
        const int index = std::stoi(m[0]);

        const auto repeats = cpy_cnt[index];
        const auto intersections = process_card(line);
        for (int i = index + 1; i <= index + intersections.size(); i++)
        {
            if (cpy_cnt.contains(i))
            {
                cpy_cnt[i]+= repeats;
            }
            else
            {
                cpy_cnt[i] = 1 + repeats; // original + repeats
            }
        }
    }

    std::cout << "Result: " << std::reduce(cpy_cnt.begin(), cpy_cnt.end(), 0, [](int init, const std::pair<const int, int> pair) {return init + pair.second; });
}

int main()
{
    second_stage();

}

