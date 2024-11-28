// Day9.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <unordered_set>
#include <iostream>
#include <unordered_map>
#include <future>
#include <set>
#include <functional>
#include <mutex>
#include "../Common/framework.h"
#include <numeric>


class Day9 final : public BaseDay
{
public:
	void get_diffs_to_zero(std::vector<std::vector<long long>>& vals)
	{
		std::vector<long long> res;
		for (auto it = vals.back().begin(); it != vals.back().end(); it++)
		{
			if (std::next(it) == vals.back().end())
				break;

			res.push_back(*(it + 1) - *it);
		}

		vals.push_back(res);

		if (std::all_of(res.begin(), res.end(), [](const auto v) {return v == 0; }))
		{
			return;
		}
		else
		{
			get_diffs_to_zero(vals);
		}
	}

	std::string fisrst_stage() override
	{
		long long summ = 0;
		for (const auto& line : input.GetInputStrByLine())
		{
			std::vector<std::vector<long long>> diffs = { GetAllNumsLL(line.cbegin(), line.cend()) };
			get_diffs_to_zero(diffs);

			diffs.rbegin()->push_back(0);
			for (auto it = diffs.rbegin() + 1; it != diffs.rend(); it++)
			{
				it->push_back(it->back() + (it - 1)->back());
			}

			summ += diffs.front().back();
		}

		return std::to_string(summ);
	}

	std::string second_stage() override
	{
		long long summ = 0;
		for (const auto& line : input.GetInputStrByLine())
		{
			std::vector<std::vector<long long>> diffs = { GetAllNumsLL(line.cbegin(), line.cend()) };
			get_diffs_to_zero(diffs);


			std::vector<long long> new_vals;
			new_vals.resize(diffs.size());


			auto new_vals_it = new_vals.rbegin();
			*new_vals_it = 0;
			new_vals_it++;

			for (auto it = diffs.rbegin() + 1; it != diffs.rend(); it++, new_vals_it++)
			{
				*new_vals_it = it->front() - *(new_vals_it - 1);
			}

			summ += new_vals.front();
		}

		return std::to_string(summ);
	}


	Day9() : BaseDay("9")
	{

	};

};

int main()
{
	Day9().Run(BaseDay::Stage::second);
}
