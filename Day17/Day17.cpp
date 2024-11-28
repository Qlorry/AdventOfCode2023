#include "Robin.hpp"
#include <iostream>
#include <unordered_set>
#include <future>
#include <set>
#include <functional>
#include <mutex>
#include "../Common/framework.h"
#include <numeric>
#include <queue>
#include <array>
#include <map>


namespace dijkstra {

    enum direction {
        NORTH = 0,
        SOUTH,
        EAST,
        WEST,
        NONE
    };

    struct position {
        size_t x;
        size_t y;

        dijkstra::direction direction;
        size_t straight;

        bool operator==(const position& pos) { return x == pos.x && y == pos.y; }
        bool operator==(const position& pos) const { return x == pos.x && y == pos.y; }

        inline std::size_t operator()(const position& c) const {
            return (c.x << 32) + (c.y << 16) + (c.straight << 8) + c.direction;
        }
    };

    struct cell {
        struct dijkstra::position position;
        int cost = 0;
        int heuristic_cost = 0;

        bool operator>(const cell& other) const { return (cost + heuristic_cost) > (other.cost + other.heuristic_cost); }
    };

    std::vector<dijkstra::direction> getDirs(dijkstra::direction dir) {
        switch (dir) {
        case dijkstra::direction::NORTH: return { dijkstra::direction::WEST, dijkstra::direction::NORTH, dijkstra::direction::EAST };
        case dijkstra::direction::SOUTH: return { dijkstra::direction::WEST, dijkstra::direction::SOUTH, dijkstra::direction::EAST };
        case dijkstra::direction::EAST: return { dijkstra::direction::NORTH, dijkstra::direction::EAST, dijkstra::direction::SOUTH };
        case dijkstra::direction::WEST: return { dijkstra::direction::NORTH, dijkstra::direction::WEST, dijkstra::direction::SOUTH };
        default: return { dijkstra::direction::WEST, dijkstra::direction::NORTH, dijkstra::direction::EAST, dijkstra::direction::SOUTH };
        }
    }

    std::vector<cell> getValidNeighbours(const position& pos, std::vector<std::vector<int>>& grid, size_t minStraight, size_t maxStraight) {
        const size_t width = grid[0].size(), height = grid.size();
        std::vector<cell> neighbours;

        for (dijkstra::direction dir : getDirs(pos.direction)) {
            if (pos.direction == dir && pos.straight >= maxStraight) continue;
            else if (pos.direction != dijkstra::direction::NONE && pos.direction != dir && pos.straight < minStraight) continue;

            switch (dir) {
            case dijkstra::direction::NORTH: {
                if (pos.y <= 0) continue;
                neighbours.push_back({ {pos.x, pos.y - 1, dijkstra::direction::NORTH, dir == pos.direction ? pos.straight + 1 : 1 }, grid[pos.y - 1][pos.x] });

                break;
            }
            case dijkstra::direction::SOUTH: {
                if (pos.y >= height - 1) continue;
                neighbours.push_back({ {pos.x, pos.y + 1, dijkstra::direction::SOUTH, dir == pos.direction ? pos.straight + 1 : 1 }, grid[pos.y + 1][pos.x] });

                break;
            }
            case dijkstra::direction::WEST: {
                if (pos.x <= 0) continue;
                neighbours.push_back({ {pos.x - 1, pos.y, dijkstra::direction::WEST, dir == pos.direction ? pos.straight + 1 : 1 }, grid[pos.y][pos.x - 1] });

                break;
            }
            case dijkstra::direction::EAST: {
                if (pos.x >= width - 1) continue;
                neighbours.push_back({ {pos.x + 1, pos.y, dijkstra::direction::EAST, dir == pos.direction ? pos.straight + 1 : 1 }, grid[pos.y][pos.x + 1] });

                break;
            }
            default: break;
            }
        }

        return neighbours;
    }

    int dijkstraSearch(std::vector<std::vector<int>>& grid, const struct position& start, const struct position& dest, size_t minStraight, size_t maxStraight, int maxScore = std::numeric_limits<int>::max()) {
        std::priority_queue<dijkstra::cell, std::vector<dijkstra::cell>, std::greater<dijkstra::cell>> queue;
        robin_hood::unordered_flat_set<dijkstra::position, dijkstra::position> visited;
        queue.push({ start, 0 });

        while (!queue.empty()) {
            const dijkstra::cell cur = queue.top();
            queue.pop();

            if (visited.contains(cur.position)) continue;
            if (cur.position == dest && cur.position.straight >= minStraight)
            {
                return cur.cost;
            }
            visited.emplace(cur.position);

            for (dijkstra::cell& neighbour : getValidNeighbours(cur.position, grid, minStraight, maxStraight)) {
                if (visited.contains(neighbour.position)) continue;

                neighbour.cost += cur.cost;

                queue.push(neighbour);
            }
        }

        return maxScore;
    }

    int aStarSearch(std::vector<std::vector<int>>& grid, const struct position& start, const struct position& dest, size_t minStraight, size_t maxStraight, int maxScore = std::numeric_limits<int>::max()) {
        std::priority_queue<dijkstra::cell, std::vector<dijkstra::cell>, std::greater<dijkstra::cell>> queue;
        robin_hood::unordered_flat_set<dijkstra::position, dijkstra::position> visited;

        queue.push({ start, 0 });

        while (!queue.empty()) {
            const dijkstra::cell cur = queue.top();
            queue.pop();

            if (visited.contains(cur.position)) continue;
            if (cur.position == dest && cur.position.straight >= minStraight)
            {
                std::cout << visited.size() << std::endl;
                return cur.cost;
            }
            visited.emplace(cur.position);

            for (dijkstra::cell& neighbour : getValidNeighbours(cur.position, grid, minStraight, maxStraight)) {
                neighbour.cost += cur.cost;
                neighbour.heuristic_cost = std::abs(static_cast<int>(neighbour.position.x) - static_cast<int>(dest.x)) + std::abs(static_cast<int>(neighbour.position.y) - static_cast<int>(dest.y));

                queue.push(neighbour);
            }
        }

        return maxScore;
    }

};

class Day17 final : public BaseDay
{
public:

    std::string fisrst_stage() override
    {
        std::vector<std::vector<int>> map;
        map.resize(input.GetInputStrByLine().size());
        int i = 0;
        for (const auto& line : input.GetInputStrByLine())
        {
            for (auto c : line)
            {
                map[i].emplace_back(static_cast<unsigned char>(c) - static_cast<unsigned char>('0'));
            }
            i++;
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto partASolution = dijkstra::aStarSearch(map, dijkstra::position{ 0, 0, dijkstra::direction::NONE, 1 }, dijkstra::position{ map[0].size() - 1, map.size() - 1 }, 1, 3);
        auto aStarSearch_end = std::chrono::high_resolution_clock::now();
        auto partSolution = dijkstra::dijkstraSearch(map, dijkstra::position{ 0, 0, dijkstra::direction::NONE, 1 }, dijkstra::position{ map[0].size() - 1, map.size() - 1 }, 1, 3);

        auto dijkstra_end = std::chrono::high_resolution_clock::now();
        std::cout << "A* == " << std::chrono::duration_cast<std::chrono::microseconds>(aStarSearch_end - start).count() << std::endl;
        std::cout << "dijkstra == " << std::chrono::duration_cast<std::chrono::microseconds>(dijkstra_end - aStarSearch_end).count()<<std::endl;

        return std::to_string(partASolution);
    }

	std::string second_stage() override
	{
		long long summ = 0;
		for (const auto& line : input.GetInputStrByLine())
		{

		}

		return std::to_string(summ);
	}


	Day17() : BaseDay("17")
	{

	};

};
int main()
{
	Day17().Run();
}
