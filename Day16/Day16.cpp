// Day16.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <queue>
#include <array>

enum class type {
    empty,
    mirror_right,
    mirror_left,
    splitter_vertical,
    splitter_horizontal
};

enum class direction {
    up,
    down,
    left,
    right
};

struct Node
{
    Node() {};
    Node(int _x, int _y, char _t) : x(_x), y(_y)
    {
        switch (_t)
        {
        case '/':
            t = type::mirror_right;
            break;
        case '\\':
            t = type::mirror_left;
            break;
        case '|':
            t = type::splitter_vertical;
            break;
        case '-':
            t = type::splitter_horizontal;
            break;
        default:
            t = type::empty;
            break;
        }
    }

    int x = 0;
    int y = 0;
    type t = type::empty;

    std::vector<std::pair<std::pair<int, int>, direction>> GetNext(direction d)
    {
        std::vector< std::pair<std::pair<int, int>, direction>> res;

        switch (t)
        {
        case type::empty:
            if (d == direction::down || d == direction::up)
            {
                res.push_back({ {y , x}, d });
            }
            else
            {
                res.push_back({ {y, x }, d });
            }
            break;
        case type::mirror_right: // '/'
            switch (d)
            {
            case direction::up:
                res.push_back({ {y, x}, direction::right });
                break;
            case direction::down:
                res.push_back({ {y, x }, direction::left });
                break;
            case direction::left:
                res.push_back({ {y, x }, direction::down });
                break;
            case direction::right:
                res.push_back({ {y, x }, direction::up });
                break;
            default:
                break;
            }
            break;
        case type::mirror_left: // '\'
            switch (d)
            {
            case direction::up:
                res.push_back({ {y, x}, direction::left });
                break;
            case direction::down:
                res.push_back({ {y, x }, direction::right });
                break;
            case direction::left:
                res.push_back({ {y, x }, direction::up });
                break;
            case direction::right:
                res.push_back({ {y, x }, direction::down });
                break;
            default:
                break;
            }
            break;
        case type::splitter_vertical:
            if (d == direction::down || d == direction::up)
            {
                res.push_back({ {y , x}, d });
            }
            else
            {
                res.push_back({ {y, x }, direction::down });
                res.push_back({ {y, x }, direction::up });
            }
            break;
        case type::splitter_horizontal:
            if (d == direction::down || d == direction::up)
            {
                res.push_back({ {y, x }, direction::left });
                res.push_back({ {y, x }, direction::right });
            }
            else
            {
                res.push_back({ {y , x}, d });

            }
            break;
        default:
            break;
        }

        for (auto& n : res)
        {
            switch (n.second)
            {
            case direction::up:
                n.first.first--;
                break;
            case direction::down:
                n.first.first++;
                break;
            case direction::left:
                n.first.second--;
                break;
            case direction::right:
                n.first.second++;
                break;
            default:
                break;
            }
        }
        return res;
    }
};
bool isValid(size_t x, size_t y, size_t rows, size_t cols) {
    return x >= 0 && x < cols && y >= 0 && y < rows;
}

class VisitMap
{
public:
    VisitMap(int rows, int cols) : m_visited(rows, std::vector<std::array<bool, 4>>(cols, { false, false, false, false }))
    {}

    bool visited_at(std::pair<int, int> pos, direction d) const
    {
        return m_visited[pos.first][pos.second][static_cast<size_t>(d)];
    }

    void add_visit(std::pair<int, int> pos, direction d)
    {
        m_visited[pos.first][pos.second][static_cast<size_t>(d)] = true;;
    }

    long long calc_visited() 
    {
        long long r = 0;
        for (auto j = 0; j < m_visited.size(); j++)
        {
            for (auto i = 0; i < m_visited[j].size(); i++)
            {
                if (visited_at({ j, i }))
                {
                    r++;
                }
            }
        }
        return r;
    }

    void Print()
    {
        
            std::string s;
        for (auto j = 0; j < m_visited.size(); j++)
        {
            for (auto i = 0; i < m_visited[j].size(); i++)
            {
                if (visited_at({ j, i }))
                {
                    s.append("#");
                }
                else
                {
                    s.append(".");
                }
            }
            s.append("\n");
        }
        std::cout << s << std::endl;
    }

private:
    bool visited_at(std::pair<int, int> pos) const
    {
        const auto& loc = m_visited[pos.first][pos.second];
        return std::any_of(loc.begin(), loc.end(), [](const bool b) {return b; });
    };

    std::vector<std::vector<std::array<bool, 4>>> m_visited;

};

VisitMap not_bfs(const std::vector<std::vector<Node>>& grid, std::pair<int, int> start = { 0,0 }, direction start_dir = direction::right) {
    int rows = grid.size();
    if (rows == 0) {
        std::cout << "Empty grid." << std::endl;
        return VisitMap(0, 0);
    }

    int cols = grid[0].size();

    VisitMap visited(rows, cols);
    std::queue<std::pair<Node, direction>> q;

    // Start BFS from every cell

    q.push({ grid[start.first][start.second], start_dir });
    visited.add_visit(start, start_dir);

    while (!q.empty()) {
        auto [current, dir] = q.front();
        q.pop();

        //std::cout << "Visited: (" << current.x << ", " << current.y << ")" << std::endl;
        auto options = current.GetNext(dir);
        for (const auto [pos, opt_dir] : options)
        {
            if (isValid(pos.first, pos.second, rows, cols) && !visited.visited_at(pos, opt_dir)) {
                q.push({ grid[pos.first][pos.second], opt_dir });
                visited.add_visit(pos, opt_dir);
            }
        }
        //visited.Print();
    }
    return visited;
}

class Day16 final : public BaseDay
{
public:

	std::string fisrst_stage() override
	{
        auto map = create_map();
        auto lines = not_bfs(map);

        return std::to_string(lines.calc_visited());
	}

	std::string second_stage() override
	{
        auto map = create_map();

        long long max = 0;


        for (auto i = 0; i < map[0].size(); i++)
        {
            max = std::max(max, not_bfs(map, { 0,i }, direction::down).calc_visited());
        }
        for (auto i = 0; i < map[0].size(); i++)
        {
            max = std::max(max, not_bfs(map, { map.size() - 1,i }, direction::up).calc_visited());
        }
        for (auto j = 0; j < map.size(); j++)
        {
            max = std::max(max, not_bfs(map, { j,0 }, direction::right).calc_visited());
        }
        for (auto j = 0; j < map.size(); j++)
        {
            max = std::max(max, not_bfs(map, { j,map[0].size()-1 }, direction::left).calc_visited());
        }

        return std::to_string(max);
	}

    std::vector<std::vector<Node>> create_map()
    {
        std::vector<std::vector<Node>> map;
        map.resize(input.GetInputStrByLine().size());
        auto j = 0;
        for (const auto& line : input.GetInputStrByLine())
        {
            int i = 0;
            for (auto c : line)
            {
                map[j].emplace_back(i, j, c);
                i++;
            }
            j++;
        }
        return map;
    }

	Day16() : BaseDay("16")
	{

	};

};

int main()
{
	Day16().Run();
}

