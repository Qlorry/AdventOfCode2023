// Day15.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

struct Node {
	std::string_view label;
	unsigned char f_length;
};

static unsigned char hash(const std::string_view& s)
{
	unsigned char res = 0;
	for (char c : s)
	{
		unsigned int temp = 0;
		temp = (static_cast<unsigned int>(res)) + static_cast<unsigned char>(c);
		temp *= 17;
		//res = temp;
		res = temp % 256;
	}
	return res;
}

class Day15 final : public BaseDay
{
public:


	std::string fisrst_stage() override
	{
		long long summ = 0;
		const auto& line = input.GetInputStrByLine()[0];
		std::string_view str(line);
		size_t pos = 0;
		while (true)
		{
			str = std::string_view(str.begin() + pos, str.end());

			auto next = str.find(',');
			auto last_it = (next == std::string::npos ? str.end() : str.begin() + next);
			
			summ += hash({ str.begin(), last_it });

			if (next == std::string::npos)
			{
				break;
			}
			pos = next+1;
		}

		return std::to_string(summ);
	}

	void do_stuff_with_lenses(const std::string_view command, std::unordered_map<unsigned char, std::list<Node>>& lense_boxes)
	{
		bool is_remove_command = command.back() == '-';

		const std::string_view label(command.begin(), command.end() - (is_remove_command ? 1 : 2));

		auto& lenses = lense_boxes[hash(label)];
		auto prev_lens = std::find_if(lenses.begin(), lenses.end(), [&label](const auto& n) {
			return n.label == label;
			});

		if (is_remove_command)
		{
			if(prev_lens != lenses.end())
			lenses.erase(prev_lens);
		}
		else
		{
			unsigned char lense_f_num = static_cast<unsigned char>(command.back()) - static_cast<unsigned char>('0');
			if (prev_lens != lenses.end())
			{
				prev_lens->f_length = lense_f_num;
			}
			else
			{
				lenses.push_back({ label, lense_f_num });
			}
		}
	}

	long long calc_power(const std::unordered_map<unsigned char, std::list<Node>>& hashmap) {
		long long res = 0;
		for (const auto& [box, lenses] : hashmap)
		{
			if (lenses.empty())
				continue;
			 
			const long long curr = box + 1;
			auto it = lenses.cbegin();
			for (auto i = 1; i <= lenses.size(); i++, ++it)
			{
				res += curr * i * it->f_length;
			}
		}
		return res;
	}

	std::string second_stage() override
	{
		std::unordered_map<unsigned char, std::list<Node>> hashmap;
		const auto& line = input.GetInputStrByLine()[0];
		std::string_view str(line);
		size_t pos = 0;
		while (true)
		{
			str = std::string_view(str.begin() + pos, str.end());

			auto next = str.find(',');
			auto last_it = (next == std::string::npos ? str.end() : str.begin() + next);

			std::string_view all_command(str.begin(), last_it);

			do_stuff_with_lenses(all_command, hashmap);

			if (next == std::string::npos)
			{
				break;
			}
			pos = next + 1;
		}
		return std::to_string(calc_power(hashmap));
	}


	Day15() : BaseDay("15")
	{

	};

};

int main()
{
	Day15().Run();
}
