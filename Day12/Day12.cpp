// Day12.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


constexpr char good = '.';
constexpr char bad = '#';
constexpr char unknown = '?';


class Day12 final : public BaseDay
{
public:

	bool validate(const std::string_view f, const std::string_view ref)
	{
		for (auto i = 0; i < f.length(); i++)
		{
			if (f[i] == ref[i])
				continue;
			if (unknown != ref[i])
				return false;
		}
		return true;
	}

	size_t Compose(const std::string& line, std::string curr_line, std::vector<int> nums, int num_ind, std::shared_ptr<std::unordered_map<unsigned long long, size_t>> position_to_combination)
	{
		if (curr_line.length() > line.length())
		{
			return 0;
		}

		if (nums.empty())
		{
			if (line.length() == curr_line.length())
			{
				return 1;
			}
			std::string fill_up_str(line.length() - curr_line.length(), good);
			return validate(fill_up_str,
				{ line.cbegin() + curr_line.length(), line.cbegin() + curr_line.length() + fill_up_str.length() });
		}

		if ((std::accumulate(nums.begin(), nums.end(), 0) + nums.size() - 1) > (line.length() - curr_line.length()))
		{
			return 0;
		}

		size_t versions = 0;

		auto shifts = line.length() - (curr_line.length() + nums[0]);
		std::string current_broken(nums[0], bad);
		for (auto i = curr_line.empty() ? 0 : 1; i <= shifts; i++)
		{
			std::string good(i, '.');
			std::string new_line = curr_line + good + current_broken;
			auto validate_size = i + nums[0];

			if (!validate(
				{ new_line.cbegin() + curr_line.length(), new_line.cbegin() + (curr_line.length() + validate_size) },
				{ line.cbegin() + curr_line.length(), line.cbegin() + (curr_line.length() + validate_size) }))
			{
				// invalid beacuse '.' does not fit in there, other will have this dot anyway => no need to check other shifts
				if (!validate(
					{ new_line.cbegin() + curr_line.length(), new_line.cbegin() + (curr_line.length() + i) },
					{ line.cbegin() + curr_line.length(), line.cbegin() + (curr_line.length() + i) }))
				{
					break;
				}
				continue;
			}
			// we know that on left there allways empty or '.', lets check right
			if (new_line.length() <= line.length() - 1 && line[new_line.length()] == bad)
			{
				continue;
			}
		

			auto key = (1000 * (curr_line.length() + i)) + num_ind;
			if (auto it = position_to_combination->find(key); it != position_to_combination->end())
			{
				versions += it->second;
			}
			else
			{
				auto combs = Compose(line, new_line, { nums.begin() + 1, nums.end() }, num_ind+1, position_to_combination);
				(*position_to_combination)[key] = combs;
				versions += combs;
			}
		}
		return versions;
	}


	std::string fisrst_stage() override
	{
		auto res = 0ull;
		for (const auto& line : input.GetInputStrByLine())
		{
			auto data_split = line.find(' ');

			auto broken_spr = GetAllNums(line.substr(data_split));

			auto springs = line.substr(0, data_split);
			auto cache = std::make_shared<std::unordered_map<unsigned long long, size_t>>();

			res += Compose(springs, "", broken_spr, 0, cache);
		}
		return std::to_string(res);
	}

	std::string second_stage() override
	{
		auto res = 0ull;
		int i = 1;

		std::vector<std::future<size_t>> ths;
		for (const auto& line : input.GetInputStrByLine())
		{
			auto data_split = line.find(' ');

			auto broken_spr = GetAllNums(line.substr(data_split));

			auto springs = line.substr(0, data_split);

			auto unfolded_springs = springs;
			auto unfolded_broken_spr = broken_spr;
			for (int i = 0; i < 4; i++)
			{
				unfolded_springs += '?' + springs;
				unfolded_broken_spr.insert(unfolded_broken_spr.end(), broken_spr.begin(), broken_spr.end());
			}
			auto cache = std::make_shared<std::unordered_map<unsigned long long, size_t>>();

			std::cout << "Processing line number " << i++ << "\n";
			res += Compose(unfolded_springs, "", unfolded_broken_spr, 0, cache);
			// 
			//ths.push_back(std::async(std::launch::async, &Day12::Compose, this, unfolded_springs, "", unfolded_broken_spr, 0, cache));
		}

		for (auto& task : ths)
		{
			std::cout << "Waiting line number " << i++ << "\n";
			res += task.get();
		}
		return std::to_string(res);
	}


	Day12() : BaseDay("12")
	{

	};

};

int main()
{
	Day12().Run(BaseDay::Stage::second);
}
