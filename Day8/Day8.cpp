// Day8.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


class Day8 final : public BaseDay
{
public:


	std::string fisrst_stage() override
	{
		const auto& inp = input.GetInputStrByLine();

		for (auto line = inp.cbegin() + 2; line != inp.cend(); line++)
		{
			// QRX = (XNN, TCJ)
			auto current = line->substr(0, 3);
			auto left = line->substr(7, 3);
			auto right = line->substr(12, 3);

			nodes[current] = { left, right };
		}

		const auto& commands = inp[0];
		size_t iterations = 0;
		std::string loc = "AAA";
		auto command = commands.cbegin();
		while (loc != "ZZZ")
		{
			iterations++;
			const auto& [left_n, right_n] = nodes[loc];
			if (*command == 'L')
			{
				loc = left_n;
			}
			else
			{
				loc = right_n;
			}

			if (std::next(command) == commands.cend())
			{
				command = commands.cbegin();
			}
			else
			{
				command++;
			}
		}

		return std::to_string(iterations);
	}

	long long mod(long long val, long long by_mod)
	{
		return (by_mod + (val % by_mod)) % by_mod;
	}

	std::string second_stage() override
	{
		const auto& inp = input.GetInputStrByLine();

		std::vector<std::string> start_nodes;
		for (auto line = inp.cbegin() + 2; line != inp.cend(); line++)
		{
			auto current = line->substr(0, 3);
			if (current.back() == 'A')
			{
				start_nodes.push_back(current);
			}

			auto left = line->substr(7, 3);
			auto right = line->substr(12, 3);

			nodes[current] = { left, right };
		}

		const auto& commands = inp[0];

		std::vector<std::future<std::pair<int, std::vector<std::string>>>> workers;
		for (auto& loc : start_nodes)
		{
			workers.push_back(
				std::async(std::launch::async, &Day8::finder, this, loc, commands)
			);
		}

		std::vector<std::pair<int, std::vector<std::string>>> sequences;
		std::vector<long long> seq_length;
		for (auto& w : workers)
		{
			auto val(w.get());
			seq_length.push_back(val.first);
			sequences.push_back(std::move(val));
		}

		auto mult_to_reach_smallest_divisible = smallestDivisible(seq_length) / sequences[0].first;
		auto first_z_pos = std::find_if(sequences[0].second.begin(), sequences[0].second.end(), [](const auto& str) {return str.back() == 'Z'; });
		auto K = std::distance(sequences[0].second.begin(), first_z_pos);

		if (K % sequences[0].first != 0)
		{
			std::cout << "Algo does not handle front shifts != seq_length!" << std::endl;
		}

		return std::to_string(mult_to_reach_smallest_divisible * sequences[0].first);
	}

	// Function to calculate the smallest divisible value for a set of numbers
	long long smallestDivisible(const std::vector<long long>& numbers) {
		long long result = 1;
		for (long long num : numbers) {
			result = std::lcm(result, num);
		}
		return result;
	}

	std::pair<int, std::vector<std::string>> finder(std::string loc, std::string commands)
	{
		auto command = commands.cbegin();
		bool found_loop = false;

		std::vector<std::string> sequence;
		while (!found_loop)
		{
			sequence.push_back(loc);
			const auto& [left_n, right_n] = nodes[loc];
			if (*command == 'L')
			{
				loc = left_n;
			}
			else
			{
				loc = right_n;
			}

			command = std::next(command);
			if (command == commands.cend())
			{
				command = commands.cbegin();


				auto same = false;
				auto seq_size = 2;
				for (; !same && seq_size * 2 <= sequence.size(); seq_size++)
				{
					same = true;
					auto last_seq_start = sequence.end() - 2 * seq_size;
					auto curr_seq_start = sequence.end() - seq_size;
					for (; curr_seq_start != sequence.end(); curr_seq_start++, last_seq_start++)
					{
						if (*last_seq_start != *curr_seq_start) {
							same = false;
							break;
						}
					}
				}
				if (same)
				{
					return { --seq_size, sequence };
				}

			}
		}
	}

	Day8() : BaseDay("8")
	{

	};

private:
	std::unordered_map<std::string, std::pair<std::string, std::string>> nodes;
};

int main()
{
	Day8().Run(BaseDay::Stage::first);
	Day8().Run(BaseDay::Stage::second);
}
