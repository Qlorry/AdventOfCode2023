#include "BaseDayTest.h"

#include "../Common/Robin.hpp"
#include "../Common/search.h"
#include <algorithm>
#include <deque>
#include <array>

namespace AdventOfCode2023 {

	class Day18 : public BaseDayTest
	{
	protected:
		Day18() : BaseDayTest(18) {
			std::cerr << "Hello!" << std::endl;
		}
	};

	size_t SimpleSolution(std::vector<std::pair<std::int64_t, std::int64_t>> vertices)
	{
		//Shoelace Formula
		//std::int64_t s1 = 0;
		//for (auto i = 0; i < vertices.size() - 1; i++)
		//{
		//	s1 += vertices[i].first * vertices[i + 1].second;
		//}

		//std::int64_t s2 = 0;
		//for (auto i = 0; i < vertices.size() - 1; i++)
		//{
		//	s2 += vertices[i].second * vertices[i + 1].first;
		//}

		//const auto inside = (std::abs(s1 - s2)) / 2;

		std::int64_t s = 0;
		for (auto i = 0; i < vertices.size() - 1; i++)
		{
			s += vertices[i].first * vertices[i + 1].second-vertices[i].second * vertices[i + 1].first;
		}
		const auto inside = std::abs(s) / 2;

		auto perimeter = 0ull;
		for (auto i = 1; i < vertices.size(); i++)
		{
			// only straight lines so one will be zoro
			perimeter += std::abs(vertices[i].first - vertices[i - 1].first + vertices[i].second - vertices[i - 1].second);
		}

		return inside + (perimeter / 2) + 1;
	}

	TEST_F(Day18, FirstStage)
	{
		std::vector<std::pair<std::int64_t, std::int64_t>> vertices;
		vertices.reserve(input.GetInputStrByLine().size());

		std::pair<std::int64_t, std::int64_t> curr_pos = { 0,0 }; // x, y
		vertices.push_back(curr_pos);

		for (const auto& line : input.GetInputStrByLine())
		{
			const auto move_by = std::stoi(line.data() + 1);

			switch (line[0])
			{
			case'R':
				curr_pos.first += move_by;
				break;
			case'L':
				
				curr_pos.first -= move_by;

				break;
			case'U':
				
				curr_pos.second -= move_by;

				break;

			case'D':
				
				curr_pos.second += move_by;

				break;

			default:
				ASSERT_TRUE(false);
			}
			vertices.push_back(curr_pos);
		}
		
		std::cout << "Digged cells: " << SimpleSolution(vertices) << std::endl;
	}

	TEST_F(Day18, SecondStage)
	{
		std::vector<std::pair<std::int64_t, std::int64_t>> vertices;
		vertices.reserve(input.GetInputStrByLine().size());

		std::pair<std::int64_t, std::int64_t> curr_pos = { 0,0 }; // x, y
		vertices.push_back(curr_pos);

		for (const auto& line : input.GetInputStrByLine())
		{
			auto hexStart = line.find('#');
			int move_by;
			std::stringstream ss;
			ss << std::hex << std::string_view(line.begin() + hexStart + 1, line.begin() + hexStart + 1 + 5);
			ss >> move_by;

			switch (line[hexStart + 1 + 5])
			{
			case'0': // R
				curr_pos.first += move_by;
				break;
			case'2': // Left
				curr_pos.first -= move_by;
				break;
			case'3': // up
				curr_pos.second -= move_by;
				break;
			case'1': // Down
				curr_pos.second += move_by;
				break;
			default:
				ASSERT_TRUE(false);
			}
			vertices.push_back(curr_pos);
		}

		std::cout << "Digged cells: " << SimpleSolution(vertices) << std::endl;
	}
}