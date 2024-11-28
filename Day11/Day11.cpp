// Day11.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <unordered_set>
#include <iostream>
#include <unordered_map>
#include <future>
#include <queue>
#include <functional>
#include <mutex>
#include "../Common/framework.h"
#include <numeric>
#include <optional>

struct Node {

	bool is_galaxy = false;


};

class Day11 final : public BaseDay
{
public:

	std::vector<std::vector<Node>> resize_map(const std::vector<std::vector<Node>>& input_map)
	{

		std::vector<std::vector<Node>> resized_map;
		for (auto line_it = input_map.begin(); line_it != input_map.end(); line_it++)
		{
			if (std::all_of(line_it->begin(), line_it->end(), [](const auto& v) {return !v.is_galaxy; }))
			{
				resized_map.push_back(std::vector<Node>(line_it->size()));
			}
			resized_map.push_back(*line_it);
		}

		std::vector<size_t> empty_cols;
		for (size_t i = 0; i < resized_map.begin()->size(); i++)
		{
			if (std::all_of(resized_map.begin(), resized_map.end(), [i](const auto& v) {return !v.at(i).is_galaxy; }))
			{
				empty_cols.push_back(i);
			}
		}

		for (auto line_it = resized_map.begin(); line_it != resized_map.end(); line_it++)
		{
			for (auto empty_space_i = 0; empty_space_i < empty_cols.size(); empty_space_i++)
			{
				line_it->insert(line_it->begin() + empty_space_i + empty_cols[empty_space_i], {false});
			}
		}
		return resized_map;
	}

	std::vector<std::pair<long long, long long>> find_galaxies(const std::vector<std::vector<Node>>& map)
	{
		std::vector<std::pair<long long, long long>> res;
		for (long long i = 0; i < map.size(); i++)
		{
			for (long long j = 0; j < map[i].size(); j++)
			{
				if (map[i][j].is_galaxy)
				{
					res.push_back({ i, j });
				}
			}
		}
		return res;
	}

	std::string fisrst_stage() override
	{
		std::vector<std::vector<Node>> input_map;
		long long summ = 0;
		input_map.resize(input.GetInputStrByLine().size());
		auto map_it = input_map.begin();
		for (const auto& line : input.GetInputStrByLine())
		{
			for (auto ch : line)
			{
				map_it->push_back({ ch == '#' });
			}
			map_it++;
		}

		auto resized_map = resize_map(input_map);
		auto gal = find_galaxies(resized_map);

		for (size_t i = 0; i < gal.size(); i++)
		{
			for (size_t j = i+1; j < gal.size(); j++)
			{
				summ += std::abs(gal[i].first - gal[j].first) + std::abs(gal[i].second - gal[j].second);
			}
		}

		return std::to_string(summ);
	}

	std::string second_stage() override
	{
		std::vector<std::vector<Node>> input_map;
		long long summ = 0;
		input_map.resize(input.GetInputStrByLine().size());
		auto map_it = input_map.begin();
		for (const auto& line : input.GetInputStrByLine())
		{
			for (auto ch : line)
			{
				map_it->push_back({ ch == '#' });
			}
			map_it++;
		}

		auto gal = find_galaxies(input_map);

		std::vector<size_t> empty_rows;
		for (size_t line = 0; line < input_map.size(); line++)
		{
			auto row = input_map.begin() + line;
			if (std::all_of(row->begin(), row->end(), [](const auto& v) {return !v.is_galaxy; }))
			{
				empty_rows.push_back(line);
			}
		}

		std::vector<size_t> empty_cols;
		for (size_t i = 0; i < input_map.begin()->size(); i++)
		{
			if (std::all_of(input_map.begin(), input_map.end(), [i](const auto& v) {return !v.at(i).is_galaxy; }))
			{
				empty_cols.push_back(i);
			}
		}


		for (size_t i = 0; i < gal.size(); i++)
		{
			for (size_t j = i + 1; j < gal.size(); j++)
			{
				const auto get_real_pos = [](long long pos, std::vector<size_t>& correction_ind) {
					auto res_pos = pos;
					for (auto ind : correction_ind)
					{
						if (pos < ind)
						{
							break;
						}
						res_pos += 1000000 - 1;
					}
					return res_pos;
				};


				auto first = gal[i];
				auto second = gal[j];

				first = { get_real_pos(first.first, empty_rows), get_real_pos(first.second, empty_cols) };
				second = { get_real_pos(second.first, empty_rows), get_real_pos(second.second, empty_cols) };

				summ += std::abs(first.first - second.first) + std::abs(first.second - second.second);
			}
		}

		return std::to_string(summ);
	}


	Day11() : BaseDay("11")
	{

	};

};

int main()
{
	Day11().Run(BaseDay::Stage::second);
}
