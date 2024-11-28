#include "pch.h"
#include "search.h"

#include <queue>

using namespace search;
std::vector<direction> search::getDirs(direction dir) {
    switch (dir) {
    case direction::NORTH: return { direction::WEST, direction::NORTH, direction::EAST };
    case direction::SOUTH: return { direction::WEST, direction::SOUTH, direction::EAST };
    case direction::EAST: return { direction::NORTH, direction::EAST, direction::SOUTH };
    case direction::WEST: return { direction::NORTH, direction::WEST, direction::SOUTH };
    default: return { direction::WEST, direction::NORTH, direction::EAST, direction::SOUTH };
    }
}

std::vector<cell> search::getValidNeighbours(const position& pos, IGrid& grid)
{
    std::vector<cell> neighbours;

    for (direction dir : getDirs(pos.direction)) {
        switch (dir) {
        case direction::NORTH: {
            if (pos.y <= grid.min_y()) continue;

            const auto new_x = pos.x;
            const auto new_y = pos.y - 1;

            if (!grid.can_go(pos.y, pos.x, new_y, new_x)) continue;

            neighbours.push_back({ { new_x, new_y, direction::NORTH }, grid.get_cost(new_y, new_x) });
            break;
        }
        case direction::SOUTH: {
            if (pos.y >= grid.max_y()) continue;
            const auto new_x = pos.x;
            const auto new_y = pos.y + 1;

            if (!grid.can_go(pos.y, pos.x, new_y, new_x)) continue;

            neighbours.push_back({ { new_x, new_y,  direction::SOUTH }, grid.get_cost(new_y, new_x) });
            break;
        }
        case direction::WEST: {
            if (pos.x <= grid.min_x()) continue;
            const auto new_x = pos.x - 1;
            const auto new_y = pos.y;

            if (!grid.can_go(pos.y, pos.x, new_y, new_x)) continue;

            neighbours.push_back({ { new_x, new_y, direction::WEST }, grid.get_cost(new_y, new_x) });
            break;
        }
        case direction::EAST: {
            if (pos.x >= grid.max_x()) continue;
            const auto new_x = pos.x + 1;
            const auto new_y = pos.y;

            if (!grid.can_go(pos.y, pos.x, new_y, new_x)) continue;

            neighbours.push_back({ { new_x, new_y, direction::EAST }, grid.get_cost(new_y, new_x) });
            break;
        }
        default: break;
        }
    }

    return neighbours;
}

int search::dijkstraSearch(IGrid& grid, const position& start, const position& dest, int maxScore)
{
    std::priority_queue<cell, std::vector<cell>, std::greater<cell>> queue;
    robin_hood::unordered_flat_set<position, position> visited;
    queue.push({ start, 0 });

    while (!queue.empty()) {
        const cell cur = queue.top();
        queue.pop();

        if (visited.contains(cur.position)) continue;
        if (cur.position == dest)
        {
            return cur.cost;
        }
        visited.emplace(cur.position);

        for (cell& neighbour : getValidNeighbours(cur.position, grid)) {
            if (visited.contains(neighbour.position)) continue;

            neighbour.cost += cur.cost;

            queue.push(neighbour);
        }
    }

    return maxScore;
}

int search::aStarSearch(IGrid& grid, const position& start, const position& dest, int maxScore)
{
    std::priority_queue<cell, std::vector<cell>, std::greater<cell>> queue;
    robin_hood::unordered_flat_set<position, position> visited;

    queue.push({ start, 0 });

    while (!queue.empty()) {
        const cell cur = queue.top();
        queue.pop();

        if (visited.contains(cur.position)) continue;
        if (cur.position == dest)
        {
            std::cout << visited.size() << std::endl;
            return cur.cost;
        }
        visited.emplace(cur.position);

        for (cell& neighbour : getValidNeighbours(cur.position, grid)) {
            neighbour.cost += cur.cost;
            neighbour.heuristic_cost = std::abs(static_cast<int>(neighbour.position.x) - static_cast<int>(dest.x)) + std::abs(static_cast<int>(neighbour.position.y) - static_cast<int>(dest.y));

            queue.push(neighbour);
        }
    }

    return maxScore;
}
