// Day6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


// Dist = hold_time * (time - hold_time)
// 
// Greatest distance will be at "d Dist/ d hold_time" = 0 
//		=> max dist at "time - 2 * hold_time = 0"
//		=> max dist at "hold_time = time / 2"

#include <iostream>
#include <map>
#include <future>
#include <functional>
#include "../Common/framework.h"

class Day6 final : public BaseDay
{
public: 

	std::string fisrst_stage() override
	{
		const auto& inp = input.GetInputStrByLine();
		time = GetAllNumsULL(inp[0].begin(), inp[0].end());
		dist = GetAllNumsULL(inp[1].begin(), inp[1].end());

		size_t res = 1;
		for (auto i = 0; i < time.size(); i++)
		{
			size_t options = 0;
			for (auto hold_time = 1; hold_time < time[i]; hold_time++)
			{
				if (covers_dist(time[i], hold_time) > dist[i])
				{
					options++;
				}
			}
			res *= options;
		}
		return std::to_string(res);
	}

	std::string second_stage() override
	{
		const auto& inp = input.GetInputStrByLine();
		auto temp_str = inp[0];
		auto end = std::remove_if(temp_str.begin(), temp_str.end(), std::isspace);
		time = GetAllNumsULL(temp_str.begin(), end);
		temp_str = inp[1];
		end = std::remove_if(temp_str.begin(), temp_str.end(), std::isspace);
		dist = GetAllNumsULL(temp_str.begin(), end);

		size_t res = 1;
		for (auto i = 0; i < time.size(); i++)
		{
			auto left_point = binarySearch(0, time[i] / 2,time[i], dist[i], true);
			auto right_point = binarySearch(time[i] / 2 + 1, time[i],time[i], dist[i], false);
			
			auto left_val = covers_dist(time[i], left_point);
			if (left_val == dist[i])
				left_point++;
			else if (left_val < dist[i])
				left_point += 2;

			auto right_val = covers_dist(time[i], right_point);
			if (right_val == dist[i])
				right_point--;
			else if (right_val < dist[i])
				right_point -= 2;

			res *= right_point - left_point + 1;
		}
		return std::to_string(res);
	}


	Day6() : BaseDay("6") 
	{

	};
private:

	size_t binarySearch(size_t start, size_t end, size_t total_time, size_t min, bool left_side) const 
	{
		size_t left = start;
		size_t right = end;

		size_t last = start;

		while (left <= right) {
			size_t mid = left + (right - left) / 2;
			last = mid;
			
			auto curr_dist = covers_dist(total_time, mid);
			if (curr_dist == min)
			{
				return mid;
			}

			if (left_side && curr_dist < min || !left_side && curr_dist > min) {
				left = mid + 1; // Target is in the right half
			}
			else {
				right = mid - 1; // Target is in the left half
			}
		}

		return last;
	}

	size_t covers_dist(size_t time, size_t hold_time) const
	{
		auto drive_time = time - hold_time;
		return hold_time * drive_time;
	}

	float req_time(size_t dist, size_t hold_time) const
	{
		return static_cast<float>(dist) / hold_time + hold_time;
	}
	std::vector<size_t> time;
	std::vector<size_t> dist;
};

int main()
{
	Day6().Run(BaseDay::Stage::second);
}

