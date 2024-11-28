#include "BaseDayTest.h"
#include "gtest/gtest.h"

#include "../Common/Robin.hpp"
#include "../Common/search.h"
#include <algorithm>
#include <deque>
#include <memory>
#include <variant>
#include <sstream>
#include <functional>
#include <array>
#include "../Common/IntIntPairHash.h"
#include <unordered_set>

namespace {
	using Map = std::vector<std::vector<bool>>;
	std::pair<std::pair<int, int>, Map> Parse(const std::vector<std::string>& input)
	{
		Map m;
		std::pair<int, int> start;
		m.reserve(input.size());
		auto row = 0;
		for (const auto& line : input)
		{
			m.push_back({});
			m[row].reserve(line.size());
			auto col = 0;

			for (auto el : line)
			{
				switch (el)
				{
				case 'S':
					start = { row, col };
				case '.':
					m[row].push_back(true);
					break;
				case '#':
				default:
					m[row].push_back(false);
					break;
				}
				col++;
			}
			row++;
		}
		return { start, m };
	}

	//using PositionsSet = std::unordered_set<std::pair<int, int>, IntIntPairHash>;
	using PositionsSet = robin_hood::unordered_set<std::pair<int, int>, IntIntPairHash>;

	PositionsSet GetNewPositions(const PositionsSet& current, const Map& map)
	{
		PositionsSet positions;
		const auto is_inside = [&map](std::pair<int, int> p) {
			if (p.first < 0 || p.second < 0)
			{
				return false;
			}
			if (p.first >= map.size() || p.second >= map[0].size())
			{
				return false;

			}
			return true;
			};

		for (const auto& p : current)
		{
			std::array<std::pair<int, int>, 4> shifts = {
				std::pair{p.first - 1, p.second},
				std::pair{p.first + 1, p.second},
				std::pair{p.first, p.second - 1},
				std::pair{p.first, p.second + 1}
			};
			for (const auto& new_pos : shifts)
			{
				if (is_inside(new_pos) && map[new_pos.first][new_pos.second])
				{
					positions.insert(new_pos);
				}
			}
		}
		return positions;
	}

	struct InfPosition
	{
		int world_row = 0;
		int world_col = 0;

		short row = 0;
		short col = 0;
	};

	inline bool operator==(const InfPosition& lhs, const InfPosition& other) { 
		return lhs.world_row == other.world_row
			&& lhs.world_col == other.world_col
			&& lhs.col == other.col
			&& lhs.row == other.row;
	}

	class InfPositionsHash {
		public:
			inline std::size_t operator()(const InfPosition& p) const {
				return (static_cast<std::size_t>(p.world_row) << 32 + 16)
					+ (static_cast<std::size_t>(p.world_col) << 32)
					+ (static_cast<std::size_t>(p.row) << 16)
					+ static_cast<std::size_t>(p.col);

			}
	};
	using InfPositionsSet = robin_hood::unordered_set<InfPosition, InfPositionsHash>;

	std::pair<std::vector<PositionsSet>, int> CalcWorstCaseToFillMap(const Map& map)
	{
		// 0, 0 turned out to be the most complex

		PositionsSet cycle_positions;
		const auto is_cycle =
			[&map](const PositionsSet& positions, const std::vector<PositionsSet>& history)
			{
				return std::find_if(history.begin(), history.end(),
					[&positions](const auto& test_pos)
					{
						if (test_pos.size() != positions.size()) return false;

						for (const auto& it : positions)
						{
							if (!test_pos.contains(it))
							{
								return false;
							}
						}
						return true;
					});
			};
		std::vector<PositionsSet> max_history;

		const auto serach = [&](std::pair<int, int> start) {
			PositionsSet positions{ start };
			int iterations = 0;
			bool cycle = false;
			std::vector<PositionsSet> history;

			while (!cycle)
			{
				iterations++;
				positions = GetNewPositions(positions, map);
				auto it = is_cycle(positions, history);
				if (it != history.end())
				{
					cycle = true;
				}
				history.push_back(positions);
			}
			if (!max_history.empty() && history.back() != max_history.back())
			{
				std::cout << "different cycle";
			}

			if (history.size() > max_history.size())
			{
				max_history = std::move(history);
			}
			};

		serach({ 0, 0 });

		auto cycle_start = is_cycle(max_history.back(), max_history);
		//for (auto row : std::array<int, 2>{ 0, static_cast<int>(map.size() - 1) })
		//{
		//	for (auto col = 0; col < map[0].size(); col++)
		//	{
		//		serach({ row, col });
		//	}
		//}

		//for (auto col : std::array<int, 2>{ 1, static_cast<int>(map[0].size() - 2) })
		//{
		//	for (auto row = 0; row < map.size(); row++)
		//	{
		//		serach({ row, col });
		//	}
		//}

		return { max_history, static_cast<int>(std::distance(max_history.cbegin(), cycle_start)) };
	}

	InfPositionsSet GetNewPositions2(const InfPositionsSet& current, const Map& map)
	{
		InfPositionsSet positions;
		const auto clamp_to_world = [&map](InfPosition& p) {
			if (p.row < 0)
			{
				p.world_row--;
				p.row = map.size() - 1;
			}
			if (p.col < 0)
			{
				p.world_col--;
				p.col = map[0].size() - 1;
			}
		
			if (p.row >= map.size())
			{
				p.world_row++;
				p.row = 0;
			}
			if (p.col >= map[0].size())
			{
				p.world_col++;
				p.col = 0;
			}
			};

		for (const auto& p : current)
		{
			std::array<InfPosition, 4> shifts = {
				InfPosition{p.world_row, p.world_col, static_cast<short>(p.row - 1), p.col},
				InfPosition{p.world_row, p.world_col, static_cast<short>(p.row + 1), p.col},
				InfPosition{p.world_row, p.world_col, p.row, static_cast<short>(p.col - 1) },
				InfPosition{p.world_row, p.world_col, p.row, static_cast<short>(p.col + 1) }
			};

			for (auto& new_pos : shifts)
			{
				clamp_to_world(new_pos);
				
				if (map[new_pos.row][new_pos.col])
				{
					try
					{
					positions.insert(new_pos);

					}
					catch (std::exception e)
					{
						std::cout << e.what();
					}
				}
			}
		}
		return positions;
	}

}

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue> 
#include <string>
#include <unordered_set>
#include <vector>
#include <cassert> 
#include <memory>

namespace second {




	constexpr int heuristic_1 = 4;
	constexpr std::size_t n_steps = 26501365;

	struct Point {
		int tile_row;
		int tile_col;
		int row;
		int col;
		std::size_t step;
		std::size_t hash;

		Point(const int tile_row = 0, const int tile_col = 0, const int row = 0, const int col = 0, const int step = 0) : tile_row(tile_row), tile_col(tile_col), row(row), col(col), step(step) {
			hash = tile_row * tile_col * 10000 + row * col;
		}

		Point operator + (const Point& p) const {
			return Point(p.tile_row + tile_row, p.tile_col + tile_col, p.row + row, p.col + col, p.step + step);
		}

		bool operator == (const Point& p) const {
			return p.tile_row == tile_row && p.tile_col == tile_col && p.row == row && p.col == col && p.step == step;
		}
		void update_hash() {
			hash = tile_row * tile_col * 10000 + row * col;
		}
	};

	struct hasher {
		std::size_t operator() (const Point& ps) const {
			return ps.hash;
		}
	};

	const std::vector<Point> motions{
	  Point(0,0,-1,0,0),
	  Point(0,0,0,1,0),
	  Point(0,0,1,0,0),
	  Point(0,0,0,-1,0),
	};

	// Should replace point with another struct here
	std::unordered_map<Point, std::size_t, hasher> temp_map;
	std::size_t helper(const std::size_t& step, const int increment, const std::size_t& n_rows) {
		std::size_t temp = (n_steps >= step) ? (n_steps - step) / n_rows : 0;
		// std::cout << temp << '\n';
		if (temp_map.find(Point(step, increment, 0, 0, 0)) != temp_map.end()) {
			return temp_map[Point(step, increment, 0, 0, 0)];
		}
		std::size_t return_val = 0;
		for (std::size_t idx = 1; idx <= temp; idx++) {
			if (((step + n_rows * idx) <= n_steps) && ((step + n_rows * idx) % 2 == (n_steps % 2))) {
				return_val += (increment == 2) ? (idx + 1) : 1;
			}
		}
		temp_map[Point(step, increment, 0, 0, 0)] = return_val;
		// std::cout << return_val << '\n';
		return return_val;
	}

	std::unordered_map<Point, std::size_t, hasher> create_count_map(const std::vector<std::string>& map, const Point& S) {
		std::queue<Point> queue;
		std::unordered_map<Point, std::size_t, hasher> count_map;
		const auto row_size = map.size();
		const auto col_size = map[0].size();
		// std::cout << row_size << ' ' << col_size << '\n';
		// exit(0);
		queue.push(S);
		// std::cout << S.row << ' ' << S.col << '\n';
		while (!queue.empty()) {
			auto current = queue.front();
			queue.pop();
			// std::cout << current.tile_row << ' ' 
			//             << current.tile_col << ' ' 
			//             << current.row << ' ' 
			//             << current.col << ' ' 
			//             << current.step << '\n';
			if (current.row < 0) {
				current.tile_row -= 1;
				current.row += row_size;
			}
			if (current.row >= row_size) {
				current.tile_row += 1;
				current.row -= row_size;
			}
			if (current.col < 0) {
				current.tile_col -= 1;
				current.col += col_size;
			}
			if (current.col >= col_size) {
				current.tile_col += 1;
				current.col -= col_size;
			}
			// current.update_hash();
			if (current.row < 0 || current.row >= row_size || current.col < 0 || current.col >= col_size || map[current.row][current.col] == '#') continue;
			if (count_map.find(Point(current.tile_row, current.tile_col, current.row, current.col, 0)) != count_map.end()) continue;
			if (std::abs(current.tile_row) > heuristic_1 || std::abs(current.tile_col) > heuristic_1) continue;
			count_map[Point(current.tile_row, current.tile_col, current.row, current.col, 0)] = current.step;
			for (const auto& motion : motions) {
				queue.emplace(current.tile_row, current.tile_col, current.row + motion.row, current.col + motion.col, current.step + 1);
			}
		}
		return count_map;
	}

}

namespace AdventOfCode2023 {

	class Day21 : public BaseDayTest
	{
	protected:
		Day21() : BaseDayTest(21) {}
	};


	TEST_F(Day21, FirstStage)
	{
		auto [start, map] = Parse(input.GetInputStrByLine());

		PositionsSet positions{ start };
		for (auto i = 0; i < 64; i++)
		{
			positions = GetNewPositions(positions, map);
		}

		for (auto row = 0; row < map.size(); row++)
		{
			for (auto col = 0; col < map[0].size(); col++)
			{
				std::cout << (positions.contains({ row, col }) ? "O" :
					map[row][col] ? "." : "#");
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;

		std::cout << "Possible positions = " << positions.size() << std::endl;
	}

	TEST_F(Day21, SecondStage)
	{
		//auto [start, map] = Parse(input.GetInputStrByLine());

		//auto [cycle_history, max_to_cycle_iterations] = CalcWorstCaseToFillMap(map);

		//InfPositionsSet positions{ {0, 0, static_cast<short>(start.first), static_cast<short>(start.second)} };


		second::Point S(0, 0, 0, 0, 0);
		std::vector<std::string> map;
		for (const auto& line : input.GetInputStrByLine())
		{
			if (line.empty()) continue;
			map.push_back(line);
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == 'S') {
					S.row = map.size() - 1;
					S.col = i;
					S.step = 0;
				}
			}
		}

		// Count map already contains the tiles in the square contained by
		// (-heuristic_1, -heuristic_1) to (heuristic_1, 1heuristic_1)
		// inclusive of the border.
		std::vector<int> OPT;
		for (int i = -second::heuristic_1; i <= second::heuristic_1; i++) {
			OPT.push_back(i);
		}
		const auto count_map = create_count_map(map, S);
		std::size_t ans = 0;
		const auto n_rows = map.size();
		const auto n_cols = map[0].size();
		for (int row = 0; row < n_rows; row++) {
			for (int col = 0; col < n_cols; col++) {
				const auto p = second::Point(0, 0, row, col, 0);
				if (count_map.find(p) != count_map.end()) {
					for (const auto tile_row : OPT) {
						for (const auto tile_col : OPT) {
							const auto step = count_map.at(second::Point(tile_row, tile_col, row, col, 0));
							// std::cout <<tile_row <<' ' << tile_col<<' ' << row<<' ' << col<<' ' << 0 << '\n';
							if ((step % 2) == (second::n_steps % 2) && step <= second::n_steps) {
								ans += 1;
							}
							if ((tile_row == -second::heuristic_1 || tile_row == second::heuristic_1) && (tile_col == -second::heuristic_1 || tile_col == second::heuristic_1)) {
								ans += second::helper(step, 2, n_rows);
							}
							else if ((tile_row == -second::heuristic_1 || tile_row == second::heuristic_1) || (tile_col == -second::heuristic_1 || tile_col == second::heuristic_1)) {
								ans += second::helper(step, 1, n_rows);
							}
						}
					}
				}
			}
		}
		std::cout << ans << '\n';
	}
}