#pragma once
#include "Robin.hpp"
namespace search
{
    enum direction {
        NORTH = 0,
        SOUTH,
        EAST,
        WEST,
        NONE
    };

    struct position {
        long long x;
        long long y;

        direction direction = direction::NONE;

        bool operator==(const position& pos) { return x == pos.x && y == pos.y; }
        bool operator==(const position& pos) const { return x == pos.x && y == pos.y; }

        inline std::size_t operator()(const position& c) const {
            return (c.x << 32) + (c.y << 8) + c.direction;
        }
    };

    struct cell {
        struct position position;
        int cost = 0;
        int heuristic_cost = 0;

        bool operator>(const cell& other) const { return (cost + heuristic_cost) > (other.cost + other.heuristic_cost); }
    };

    struct IGrid {
        virtual int get_cost(long long y, long long x) const = 0;
        virtual int can_go(long long from_y, long long from_x, long long to_y, long long to_x) const = 0;

        virtual long long min_x() const = 0;
        virtual long long min_y() const = 0;

        virtual long long max_x() const = 0;
        virtual long long max_y() const = 0;
    };

    std::vector<direction> getDirs(direction dir);

    std::vector<cell> getValidNeighbours(
        const position& pos, 
        IGrid& grid);

    int dijkstraSearch(
        IGrid& grid, 
        const struct position& start, const struct position& dest, 
        int maxScore = std::numeric_limits<int>::max());

    int aStarSearch(
        IGrid& grid, 
        const struct position& start, const struct position& dest, 
        int maxScore = std::numeric_limits<int>::max());
};

