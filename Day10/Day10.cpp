// Day10.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

enum class pipe_type {
	any,
	none,
	visited,
	vertical,
	horizontal,
	top_to_right,
	top_to_left,
	bot_to_left,
	bot_to_right
};

enum class direction {
	top,
	bot,
	left,
	right
};

direction GetOposit(direction d)
{
	switch (d)
	{
	case direction::top:
		return direction::bot;
	case direction::bot:
		return direction::top;
	case direction::left:
		return direction::right;
	case direction::right:
		return direction::left;
	default:
		break;
	}
}


class Node {
public:
	Node(char type, size_t _x, size_t _y) : x(_x), y(_y)
	{
		switch (type)
		{
		case '|':
			m_type = pipe_type::vertical;
			break;
		case '-':			
			m_type = pipe_type::horizontal;
			break;
		case 'L':
			m_type = pipe_type::top_to_right;
			break;
		case 'J':
			m_type = pipe_type::top_to_left;
			break;
		case '7':
			m_type = pipe_type::bot_to_left;
			break;
		case 'F':
			m_type = pipe_type::bot_to_right;
			break;
		case 'S':
			m_type = pipe_type::any;
			break;
		default:
			m_type = pipe_type::none;
			break;
		}
	};
	Node(pipe_type type, size_t _x, size_t _y) : m_type(type), x(_x), y(_y) {};

	bool CanConnect(std::shared_ptr<Node> other, direction d)
	{
		if (!CanConnect(d))
		{
			return false;
		}
		if(!other->CanConnect(GetOposit(d)))
		{
			return false;
		}
		return true;
	}

	void AddConnection(direction d)
	{
		if (!prev.has_value())
		{
			prev = d;
		}
		else
		{
			next = d;
		}
	}

	bool loop = false;

	long long x;
	long long y;

	pipe_type GetType()
	{
		return m_type;
	}

	void RecalcType()
	{
		if (prev == direction::top)
		{
			if (next == direction::bot)
			{
				m_type = pipe_type::vertical;
			}
			else if (next == direction::left)
			{
				m_type = pipe_type::top_to_left;
			}
			else if (next == direction::right)
			{
				m_type = pipe_type::top_to_right;
			}
		}
		if (prev == direction::bot)
		{
			if (next == direction::top)
			{
				m_type = pipe_type::vertical;
			}
			else if (next == direction::left)
			{
				m_type = pipe_type::bot_to_left;
			}
			else if (next == direction::right)
			{
				m_type = pipe_type::bot_to_right;
			}
		}
		if (prev == direction::left)
		{
			if (next == direction::top)
			{
				m_type = pipe_type::top_to_left;
			}
			else if (next == direction::right)
			{
				m_type = pipe_type::horizontal;
			}
			else if (next == direction::bot)
			{
				m_type = pipe_type::bot_to_left;
			}
		}
		if (prev == direction::right)
		{
			if (next == direction::top)
			{
				m_type = pipe_type::top_to_right;
			}
			if (next == direction::left)
			{
				m_type = pipe_type::horizontal;
			}
			if (next == direction::bot)
			{
				m_type = pipe_type::bot_to_right;
			}
		}
	}

private:
	bool CanConnect(direction d)
	{
		if (m_type == pipe_type::any)
		{
			return true;
		}
		if (m_type == pipe_type::none)
		{
			return false;
		}
		switch (d)
		{
		case direction::top:
			if (m_type == pipe_type::vertical
				|| m_type == pipe_type::top_to_right
				|| m_type == pipe_type::top_to_left)
			{
				return true;
			}
			return false;
		case direction::bot:
			if (m_type == pipe_type::vertical
				|| m_type == pipe_type::bot_to_right
				|| m_type == pipe_type::bot_to_left)
			{
				return true;
			}
			return false;
		case direction::left:
			if (m_type == pipe_type::horizontal
				|| m_type == pipe_type::top_to_left
				|| m_type == pipe_type::bot_to_left)
			{
				return true;
			}
			return false;
		case direction::right:
			if (m_type == pipe_type::horizontal
				|| m_type == pipe_type::top_to_right
				|| m_type == pipe_type::bot_to_right)
			{
				return true;
			}
			return false;
		default:
			return false;
		}
	}
	pipe_type m_type;
	std::optional<direction> prev = { };
	std::optional<direction> next = { };
};

bool isValid(size_t x, size_t y, size_t rows, size_t cols) {
	return x >= 0 && x < cols && y >= 0 && y < rows;
}

void PrintMap(std::vector<std::vector<std::shared_ptr<Node>>>& map)
{
	for (auto line : map)
	{
		std::string line_s;
		char tmp[2];
		tmp[0] = '\0';
		tmp[1] = '\0';

		for (auto node : line)
		{
			switch (node->GetType())
			{
			case pipe_type::any:
				line_s.append("S");
				break;

			case pipe_type::none:
				line_s.append(".");
				break;

			case pipe_type::vertical:
				line_s.append("|");
				break;

			case pipe_type::horizontal:
				line_s.append("-");
				break;

			case pipe_type::top_to_right:
				tmp[0] = char(192);
				line_s.append(tmp);
				break;

			case pipe_type::top_to_left:
				tmp[0] = char(217);
				line_s.append(tmp);
				break;

			case pipe_type::bot_to_left:
				tmp[0] = char(191);
				line_s.append(tmp);
				break;

			case pipe_type::bot_to_right:
				tmp[0] = char(218);
				line_s.append(tmp);
				break;
			default:
				break;
			}

		}

		std::cout << line_s << "\n";
	}
	std::cout << "\n";

}

void PrintVisitMap(std::vector<std::vector<std::shared_ptr<Node>>> map, const std::vector<std::vector<bool>>& not_in_loop)
{
	for (size_t i = 0; i < not_in_loop.size(); ++i) {
		for (size_t j = 0; j < not_in_loop[0].size(); j++) {
			if (map[i][j]->GetType() == pipe_type::none && not_in_loop[i][j])
			{
				map[i][j] = std::make_shared<Node>(pipe_type::any, map[i][j]->x, map[i][j]->y);
			}
		}
	}
	PrintMap(map);
}

std::vector<std::vector<bool>> bfs(const std::vector<std::vector<std::shared_ptr<Node>>>& grid) {
	size_t rows = grid.size();
	if (rows == 0) {
		std::cout << "Empty grid." << std::endl;
		return {};
	}

	size_t cols = grid[0].size();
	std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

	std::queue<std::shared_ptr<Node>> q;
	const auto can_go = [](std::shared_ptr<Node> n) {
		return n->GetType() == pipe_type::none;
	};
	// Start BFS from every cell
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; j += (i == 0 || i == rows - 1) ? 1 : cols - 1) {
			auto t = grid[i][j]->GetType();
			if (t != pipe_type::none)
			{
				continue;
			}

			if (!visited[i][j]) {
				q.push(grid[i][j]);
				visited[i][j] = true;

				while (!q.empty()) {

					auto current = q.front();
					q.pop();

					//std::cout << "Visited: (" << current->y << ", " << current->x << ")\n";

					// Add neighbors to the queue
					for (int dx = -1; dx <= 1; ++dx) {
						for (int dy = -1; dy <= 1; ++dy) {
							if (std::abs(dx) > 0 && std::abs(dy) > 0)
							{
								continue;
							}

							int newX = current->x + dx;
							int newY = current->y + dy;

							if (isValid(newX, newY, rows, cols) && !visited[newY][newX] && can_go(grid[newY][newX])) {
								q.push(grid[newY][newX]);
								visited[newY][newX] = true;
							}
						}
					}
				}
			}
		}
	}

	return visited;
}


class Day10 final : public BaseDay
{
public:

	std::string fisrst_stage() override
	{
		std::vector<std::vector<std::shared_ptr<Node>>> map;
		map.resize(input.GetInputStrByLine().size());

		size_t index = 0;
		std::pair<size_t, size_t> start = {0, 0};
		for (const auto& line : input.GetInputStrByLine())
		{
			for (auto ch : line)
			{
				map[index].push_back(std::make_shared<Node>(ch, map[index].size(), index));
				if(ch =='S')
					start = { index, map[index].size()-1 };
			}
			index++;
		}

		auto line_it = map.begin() + start.first;
		auto pipe_it = line_it->begin() + start.second;
		auto start_node = *pipe_it;
		size_t steps = 0;
		std::optional<direction> prev_dir;
		do {
			auto top_pipe = line_it == map.begin() ? nullptr
				: *((line_it - 1)->begin() + std::distance(line_it->begin(), pipe_it));

			auto bot_pipe = line_it+1 == map.end() ? nullptr
				: *((line_it + 1)->begin() + std::distance(line_it->begin(), pipe_it));

			auto left_pipe = pipe_it == line_it->begin() ? nullptr
				: *(pipe_it-1);

			auto right_pipe = pipe_it+1 == line_it->end() ? nullptr
				: *(pipe_it + 1);

			// go up
			if ((prev_dir.has_value() ? prev_dir != direction::bot : true) && top_pipe
				&& (*pipe_it)->CanConnect(top_pipe, direction::top))
			{
				prev_dir = direction::top;

				(*pipe_it)->AddConnection(direction::top);
				top_pipe->AddConnection(GetOposit(direction::top));

				pipe_it = ((line_it - 1)->begin() + std::distance(line_it->begin(), pipe_it));
				line_it--;
			}
			// go right
			else if ((prev_dir.has_value() ? prev_dir != direction::left : true) && right_pipe
				&& (*pipe_it)->CanConnect(right_pipe, direction::right))
			{
				prev_dir = direction::right;

				(*pipe_it)->AddConnection(direction::right);
				right_pipe->AddConnection(GetOposit(direction::right));

				pipe_it++;
			}
			// go bot
			else if ((prev_dir.has_value() ? prev_dir != direction::top : true) && bot_pipe
				&& (*pipe_it)->CanConnect(bot_pipe, direction::bot))
			{
				prev_dir = direction::bot;

				(*pipe_it)->AddConnection(direction::bot);
				bot_pipe->AddConnection( GetOposit(direction::bot));

				pipe_it = ((line_it + 1)->begin() + std::distance(line_it->begin(), pipe_it));
				line_it++;
			}
			//go left
			else if ((prev_dir.has_value() ? prev_dir != direction::right : true) && left_pipe
				&& (*pipe_it)->CanConnect(left_pipe, direction::left))
			{
				prev_dir = direction::left;

				(*pipe_it)->AddConnection(direction::left);
				left_pipe->AddConnection( GetOposit(direction::left));
				
				pipe_it--;
			}
			else
			{
				std::cout << "Panic!";
			}

			steps++;

		} while ((*pipe_it) != start_node);

		return std::to_string(steps/2);
	}
	std::string second_stage() override
	{
		std::vector<std::vector<std::shared_ptr<Node>>> map;
		map.resize(input.GetInputStrByLine().size());

		size_t index = 0;
		std::pair<size_t, size_t> start = { 0, 0 };
		for (const auto& line : input.GetInputStrByLine())
		{
			for (auto ch : line)
			{
				map[index].push_back(std::make_shared<Node>(ch, map[index].size(), index));
				if (ch == 'S')
					start = { index, map[index].size() - 1 };
			}
			index++;
		}

		auto line_it = map.begin() + start.first;
		auto pipe_it = line_it->begin() + start.second;
		auto start_node = *pipe_it;
		std::optional<direction> prev_dir;
		do {
			(*pipe_it)->loop = true;

			auto top_pipe = line_it == map.begin() ? nullptr
				: *((line_it - 1)->begin() + std::distance(line_it->begin(), pipe_it));

			auto bot_pipe = line_it + 1 == map.end() ? nullptr
				: *((line_it + 1)->begin() + std::distance(line_it->begin(), pipe_it));

			auto left_pipe = pipe_it == line_it->begin() ? nullptr
				: *(pipe_it - 1);

			auto right_pipe = pipe_it + 1 == line_it->end() ? nullptr
				: *(pipe_it + 1);

			// go up
			if ((prev_dir.has_value() ? prev_dir != direction::bot : true) && top_pipe
				&& (*pipe_it)->CanConnect(top_pipe, direction::top))
			{
				prev_dir = direction::top;

				(*pipe_it)->AddConnection(direction::top);
				top_pipe->AddConnection(GetOposit(direction::top));

				pipe_it = ((line_it - 1)->begin() + std::distance(line_it->begin(), pipe_it));
				line_it--;
			}
			// go right
			else if ((prev_dir.has_value() ? prev_dir != direction::left : true) && right_pipe
				&& (*pipe_it)->CanConnect(right_pipe, direction::right))
			{
				prev_dir = direction::right;

				(*pipe_it)->AddConnection(direction::right);
				right_pipe->AddConnection(GetOposit(direction::right));

				pipe_it++;
			}
			// go bot
			else if ((prev_dir.has_value() ? prev_dir != direction::top : true) && bot_pipe
				&& (*pipe_it)->CanConnect(bot_pipe, direction::bot))
			{
				prev_dir = direction::bot;

				(*pipe_it)->AddConnection(direction::bot);
				bot_pipe->AddConnection(GetOposit(direction::bot));

				pipe_it = ((line_it + 1)->begin() + std::distance(line_it->begin(), pipe_it));
				line_it++;
			}
			//go left
			else if ((prev_dir.has_value() ? prev_dir != direction::right : true) && left_pipe
				&& (*pipe_it)->CanConnect(left_pipe, direction::left))
			{
				prev_dir = direction::left;

				(*pipe_it)->AddConnection(direction::left);
				left_pipe->AddConnection(GetOposit(direction::left));

				pipe_it--;
			}
			else
			{
				std::cout << "Panic!";
			}
		} while ((*pipe_it) != start_node);

		PrintMap(map);

		auto clear_map = map;

		for (auto& line : clear_map)
		{
			for (auto& node : line)
			{
				if (!node->loop)
				{
					node = std::make_shared<Node>(pipe_type::none, node->x, node->y);
				}
				if (node->GetType() == pipe_type::any)
				{
					node->RecalcType();
				}
			}
		}


		std::vector<std::vector<std::shared_ptr<Node>>>  search_map(map.size() * 3);
		auto search_m_it = search_map.begin();
		size_t line_index = 0;
		for (auto& line : clear_map)
		{
			for (int i = 0; i < 3; i++)
			{
				search_m_it->reserve(line.size() * 3);

				size_t node_index = 0;
				for (auto& node : line)
				{
					switch (node->GetType())
					{
					case pipe_type::any:
						break;
					case pipe_type::none:
						if (i == 0 || i == 2)
						{
							for (int j = 0; j < 3; j++)
							{
								search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + j, line_index * 3 + i));
							}
						}
						else
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::vertical:

						if (i == 0 || i == 2)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::vertical, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						else
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::horizontal:
						if (i == 0 || i == 2)
						{
							for (int j = 0; j < 3; j++)
							{
								search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + j, line_index * 3 + i));
							}
						}
						else
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::top_to_right:
						if (i == 0)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::vertical, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 1)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 2)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::top_to_left:
						if (i == 0)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::vertical, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 1)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 2)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::bot_to_left:
						if (i == 0)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));

						}
						if (i == 1)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 2)
						{

							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::vertical, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;

					case pipe_type::bot_to_right:
						if (i == 0)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));

						}
						if (i == 1)
						{
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							node->x = node_index * 3 + 1;
							node->y = line_index * 3 + 1;
							search_m_it->push_back(node);
							search_m_it->push_back(std::make_shared<Node>(pipe_type::horizontal, node_index * 3 + 2, line_index * 3 + i));
						}
						if (i == 2)
						{

							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::vertical, node_index * 3 + 1, line_index * 3 + i));
							search_m_it->push_back(std::make_shared<Node>(pipe_type::none, node_index * 3 + 2, line_index * 3 + i));
						}
						break;
					default:
						break;
					}

					node_index++;
				}

				search_m_it++;
			}

			line_index++;
		}

		PrintMap(search_map);

		auto not_in_loop = bfs(search_map);

		PrintVisitMap(search_map, not_in_loop);

		std::vector<std::vector<bool>> clear_not_in_loop(clear_map.size(), std::vector<bool>(clear_map[0].size(), false));
		auto enclosed_pos = 0ull;
		for (size_t i = 0; i < clear_map.size(); ++i) {
			for (size_t j = 0; j < clear_map[0].size(); j++) {

				clear_not_in_loop[i][j] = not_in_loop[i * 3 + 1][j * 3 + 1];
				if (clear_map[i][j]->GetType() == pipe_type::none && !not_in_loop[i * 3 + 1][j * 3 + 1])
				{
					enclosed_pos++;
				}
			}
		}

		PrintVisitMap(clear_map, clear_not_in_loop);

		return std::to_string(enclosed_pos);
	}

	Day10() : BaseDay("10")
	{

	};

};


int main()
{
	Day10().Run(BaseDay::Stage::first);
	Day10().Run(BaseDay::Stage::second);
}
