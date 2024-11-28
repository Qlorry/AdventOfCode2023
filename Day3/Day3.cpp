// Day3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <regex>
#include <map>

#include "../Common/framework.h"
#include <unordered_map>


bool is_serial_num(const std::vector<std::string>& mat, size_t line, size_t pos, int num_len)
{
    // One after digit
    if (pos + num_len < mat[line].size())
    {
        num_len++;
    }

    // One before digit
    if (pos > 0)
    {
        pos--;
        num_len++;
    }

    // on top
    if (line > 0)
    {
        const auto& search_begin = mat[line - 1].cbegin() + pos;
        const auto& search_end = mat[line - 1].cbegin() + pos + num_len;
        const auto it = std::find_if(search_begin, search_end, [](char c) {return c != '.'; });
        if (it != search_end)
            return true;
    }

    // Same line
    const auto& search_begin = mat[line].cbegin() + pos;
    const auto& search_end = mat[line].cbegin() + pos + num_len;
    const auto it = std::find_if(search_begin, search_end, [](char c) {
        return c != '.' && !std::isdigit(c);
        });
    if (it != search_end)
        return true;

    // Under
    if (line < mat.size() - 1)
    {
        const auto& search_begin = mat[line + 1].cbegin() + pos;
        const auto& search_end = mat[line + 1].cbegin() + pos + num_len;
        const auto it = std::find_if(search_begin, search_end, [](char c) {return c != '.'; });
        if (it != search_end)
            return true;
    }

    return false;
}


bool is_gear_num(const std::vector<std::string>& mat, size_t line, size_t pos, int num_len)
{
    // One after digit
    if (pos + num_len < mat[line].size())
    {
        num_len++;
    }

    // One before digit
    if (pos > 0)
    {
        pos--;
        num_len++;
    }

    // on top
    if (line > 0)
    {
        const auto& search_begin = mat[line - 1].cbegin() + pos;
        const auto& search_end = mat[line - 1].cbegin() + pos + num_len;
        const auto it = std::find_if(search_begin, search_end, [](char c) {return c == '*'; });
        if (it != search_end)
            return true;
    }

    // Same line
    const auto& search_begin = mat[line].cbegin() + pos;
    const auto& search_end = mat[line].cbegin() + pos + num_len;
    const auto it = std::find_if(search_begin, search_end, [](char c) {
        return c == '*';
        });
    if (it != search_end)
        return true;

    // Under
    if (line < mat.size() - 1)
    {
        const auto& search_begin = mat[line + 1].cbegin() + pos;
        const auto& search_end = mat[line + 1].cbegin() + pos + num_len;
        const auto it = std::find_if(search_begin, search_end, [](char c) {return c == '*'; });
        if (it != search_end)
            return true;
    }

    return false;
}

void first_stage() 
{
    PuzzleInput i("../Day3");

    const auto& input_mat = i.GetInputStrByLine();
    int summ = 0;

    for (size_t line = 0; line < input_mat.size(); line++)
    {
        const auto& line_data = input_mat[line];
        std::regex index_r("\\d+");
        std::smatch m;

        std::string::const_iterator searchStart(line_data.cbegin());
        while (std::regex_search(searchStart, line_data.cend(), m, index_r))
        {
            auto num_start = std::distance(line_data.cbegin(), searchStart + m.position());

            if (is_serial_num(input_mat, line, num_start, m.length()))
            {
                summ += std::stoi(m[0]);
            }

            searchStart = m.suffix().first;
        }
    }
    std::cout << "Result: " << summ;

}

bool doRangesIntersect(size_t start1, size_t end1, size_t start2, size_t end2) {
    // Check if one range is completely before the other
    if (end1 < start2 || end2 < start1) {
        return false;
    }

    // Ranges overlap
    return true;
}

void second_stage()
{
    PuzzleInput i("../Day3");

    const auto& input_mat = i.GetInputStrByLine();
    int summ = 0;

    std::unordered_map<size_t, std::vector<std::pair<size_t, size_t>>> gear_numbers;
    for (size_t line = 0; line < input_mat.size(); line++)
    {
        const auto& line_data = input_mat[line];
        std::regex index_r("\\d+");
        std::smatch m;

        std::string::const_iterator searchStart(line_data.cbegin());
        while (std::regex_search(searchStart, line_data.cend(), m, index_r))
        {
            auto num_start = std::distance(line_data.cbegin(), searchStart + m.position());

            if (is_gear_num(input_mat, line, num_start, m.length()))
            {
                gear_numbers[line].emplace_back(num_start, num_start + m.length() - 1);
            }

            searchStart = m.suffix().first;
        }
    }

    for (size_t line = 0; line < input_mat.size(); line++)
    {
        const auto& line_data = input_mat[line];

        for(auto it = std::find(line_data.cbegin(), line_data.cend(), '*'); it != line_data.cend(); it = std::find(++it, line_data.cend(), '*'))
        {
            auto star_pos = std::distance(line_data.cbegin(), it);

            int number_of_gears = 0;
            int ratio = 1;


            const auto check_line_and_continue = [&](size_t line)
            {
                auto& nums_on_top_line = gear_numbers[line];
                for (const auto& num : nums_on_top_line)
                {
                    if (doRangesIntersect(num.first, num.second, star_pos - 1, star_pos + 1))
                    {
                        number_of_gears++;
                        const auto line_start = input_mat[line].begin();
                        ratio *= std::stoi(std::string(line_start + num.first, line_start + num.second + 1));
                    }
                    if (number_of_gears == 2)
                    {
                        break;
                    }
                }

                if (number_of_gears == 2)
                {
                    summ += ratio;
                    return false;
                }
                return true;
            };

            // top
            if (line > 0 && !check_line_and_continue(line - 1))
            {
                continue;
            }

            //same line
            if (!check_line_and_continue(line))
            {
                continue;
            }
            
            //under line
            if (line < input_mat.size() - 1 && !check_line_and_continue(line+1))
            {
                continue;
            }
        }
    }
    std::cout << "Result: " << summ;

}


int main()
{
    second_stage();
}
