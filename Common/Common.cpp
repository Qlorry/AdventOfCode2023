// Common.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

//#include <boost/program_options.hpp>
//namespace po = boost::program_options;

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <regex>


PuzzleInput::PuzzleInput(std::string folder_name, std::string file_name) {
    //const std::string input_key = "puzzle-input";
    //const std::string input_name = input_key + ", pi";

    //po::options_description desc("Allowed options");
    //desc.add_options()
    //    (input_name.c_str(), po::value<std::string>(), "input string")
    //    ;

    //po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    //po::notify(vm);

    //try {
    //    m_input_string = vm.at(input_key).as<std::string>();
    //}
    //catch (const std::out_of_range& ex)
    //{
    //    std::puts("No input string for puzzle!");
    //    m_input_string = "";
    //}

    ReadInput(folder_name, file_name);
}

bool PuzzleInput::ReadInput(std::string folder_name, std::string file_name)
{
    std::ifstream file(folder_name + file_name);
        
    if (!file.is_open())
    {
        std::ofstream file(folder_name + file_name);
        file << std::endl;
        file.close();
        return false;
    }
    std::string str;
    while (std::getline(file, str))
    {
        m_input_lines.push_back(str);
        m_input_string += str + "\n";
    }
    return true;
}

void PuzzleInput::Clear()
{
    m_input_lines.clear();
    m_input_string.clear();
}

const std::vector<std::string>& PuzzleInput::GetInputStrByLine() const
{
    return m_input_lines;

}

const std::string& PuzzleInput::GetInputStr() const
{
    return m_input_string;
}

std::vector<int> GetAllNums(std::string::const_iterator begin, std::string::const_iterator end)
{
    std::regex index_r("\\d+");
    std::smatch m;
    std::vector<int> res;
    while (std::regex_search(begin, end, m, index_r))
    {
        int val = std::stoi(m[0]);
        res.push_back(val);
        begin = m.suffix().first;
    }
    return res;
}

std::vector<std::size_t> GetAllNumsULL(std::string::const_iterator begin, std::string::const_iterator end)
{
    std::regex index_r("\\d+");
    std::smatch m;
    std::vector<std::size_t> res;
    while (std::regex_search(begin, end, m, index_r))
    {
        std::size_t val = std::stoull(m[0]);
        res.push_back(val);
        begin = m.suffix().first;
    }
    return res;
}

std::vector<long long> GetAllNumsLL(std::string::const_iterator begin, std::string::const_iterator end)
{
    std::regex index_r("-?\\d+(\\.\\d+)?");
    std::smatch m;
    std::vector<long long> res;
    while (std::regex_search(begin, end, m, index_r))
    {
        long long val = std::stoll(m[0]);
        res.push_back(val);
        begin = m.suffix().first;
    }
    return res;
}

std::vector<int> GetAllNums(const std::string& s) { return GetAllNums(s.cbegin(), s.cend()); }

std::vector<std::size_t> GetAllNumsULL(const std::string& s) { return GetAllNumsULL(s.cbegin(), s.cend()); }

std::vector<long long> GetAllNumsLL(const std::string& s) { return GetAllNumsLL(s.cbegin(), s.cend()); }
