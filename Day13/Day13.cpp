// Day13.cpp : This file contains the 'main' function. Program execution begins and ends there.
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



class Day13 final : public BaseDay
{
public:
	void Print(std::vector<std::string>& m)
	{
		for (auto& s : m)
			std::cout << s << "\n";
		std::cout << "\n";
	}

	std::string fisrst_stage() override
	{
		auto res = 0ull;
		std::vector<std::string> map;


		const auto check = [](const auto& map, int i, int j) {
			while (i < j)
			{
				i++;
				j--;
				if (map[i] != map[j])
				{
					return false;
				}
			}
			return true;
		};
		int i = 0;
		for (const auto& line : input.GetInputStrByLine())
		{
			if (!line.empty())
			{
				map.push_back(line);
				continue;
			}
			std::cout << "Processing map # " << ++i<<"\n";

			//Print(map);

			int h_pos = 0;
			for (auto i = 0; i < map.size()-1; i++)
			{
				if (map[i] == map[i + 1])
				{
						bool found = true;
					for (int j = i + 1, k = i; k >= 0 && j < map.size(); --k, ++j)
					{

						if (map[j] != map[k])
						{
							found = false;
							break;
						}
					}
					if (found)
					{
						h_pos = i + 1;
						break;
					}
				}
			}

			std::vector<std::string> map_rotated;
			map_rotated.resize(map[0].size());
			for (int i = map.size()-1; i >= 0; i--)
			{
				for (auto j = 0; j < map[i].size(); j++)
				{
					map_rotated[j].append(1, map[i][j]);
				}
			}

			int v_pos = 0;
			for (auto i = 0; i < map_rotated.size() - 1; i++)
			{
				if (map_rotated[i] == map_rotated[i + 1])
				{
					bool found = true;
					for (int j = i + 1, k = i; k >= 0 && j < map_rotated.size(); --k, ++j)
					{

						if (map_rotated[j] != map_rotated[k])
						{
							found = false;
							break;
						}
					}
					if (found)
					{
						v_pos = i + 1;
						break;
					}
				}
			}


			res += v_pos;
			res += 100 * h_pos;

			map.clear();
			map_rotated.clear();
		}
		return std::to_string(res);
	}

	int get_diff_size(const auto& map, int i, int j)
	{
		int corrections = 0;
		for (auto n = 0; n < map[j].length(); n++)
		{
			if (map[i][n] != map[j][n])
			{
				corrections++;
			}
		}
		return corrections;
	}

	std::string second_stage() override
	{
		auto res = 0ull;
		std::vector<std::string> map;


		const auto check = [](const auto& map, int i, int j, bool& correction_used) {
			bool found = true;
			for (; i >= 0 && j < map.size(); --i, ++j)
			{
				if (map[j] != map[i])
				{
					if (correction_used)
					{
						found = false;
						break;
					}
					int corrections = 0;
					for (auto n = 0; n < map[j].length(); n++)
					{
						if (map[j][n] != map[i][n])
						{
							correction_used = true;
							corrections++;
							continue;
						}
					}

					if (corrections > 1)
					{
						found = false;
						break;
					}
				}
			}

			return found;
		};
		int i = 0;
		for (const auto& line : input.GetInputStrByLine())
		{
			if (!line.empty())
			{
				map.push_back(line);
				continue;
			}
			std::cout << "Processing map # " << ++i << "\n";

			//Print(map);

			int h_pos = 0;
			for (auto i = 0; i < map.size() - 1; i++)
			{
				auto diff = get_diff_size(map, i, i + 1);
				if (diff <= 1)
				{
					bool corr_used = diff == 1;
					bool found = check(map, i-1, i+2, corr_used);
					if (found && corr_used)
					{
						h_pos = i + 1;
						break;
					}
				}
			}

			std::vector<std::string> map_rotated;
			map_rotated.resize(map[0].size());
			for (int i = map.size() - 1; i >= 0; i--)
			{
				for (auto j = 0; j < map[i].size(); j++)
				{
					map_rotated[j].append(1, map[i][j]);
				}
			}

			int v_pos = 0;
			for (auto i = 0; i < map_rotated.size() - 1; i++)
			{
				auto diff = get_diff_size(map_rotated, i, i + 1);
				if (diff <= 1)
				{
					bool corr_used = diff == 1;
					bool found = check(map_rotated, i - 1, i + 2, corr_used);
					if (found && corr_used)
					{
						v_pos = i + 1;
						break;
					}
				}
			}

			std::cout << "Vertical " << v_pos << " Horizontal " << h_pos << "\n";

			res += v_pos;
			res += 100 * h_pos;

			map.clear();
			map_rotated.clear();
		}
		return std::to_string(res);
	}


	Day13() : BaseDay("13")
	{

	};

};

int main()
{
	Day13().Run(BaseDay::Stage::second);
}
