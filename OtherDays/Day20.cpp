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
#include <optional>
#include <numeric>

namespace {

	enum class Signal : bool
	{
		Hi = true,
		Lo = false
	};

	class FlipFlop
	{
	public:

		std::optional<Signal> apply(const Signal s)
		{
			if (s == Signal::Hi)
			{
				return {};
			}
			_on = !_on;
			return static_cast<Signal>(_on);
		}

		bool operator ==(const FlipFlop& other)
		{
			return _on == other._on;
		}

	private:
		bool _on = false;
	};

	class Conjunction
	{
	public:
		void add_input(const std::string& input)
		{
			_inputs[input] = Signal::Lo;
		}

		Signal apply(const std::string& from, const Signal s)
		{
			_inputs.at(from) = s;
			
			return AllHi() ? Signal::Lo : Signal::Hi;
		}

		bool operator ==(const Conjunction& other)
		{
			if (_inputs.size() != other._inputs.size())
			{
				return false;
			}
			for (const auto& input : _inputs)
			{
				const auto it = other._inputs.find(input.first);
				if (it == other._inputs.end())
				{
					return false;
				}

				if (input.second != it->second)
				{
					return false;
				}
			}
			return true;
		}
	private:
		bool AllHi()
		{
			return std::all_of(_inputs.begin(), _inputs.end(),
				[](const auto& pair) { return pair.second == Signal::Hi; });
		}
		robin_hood::unordered_flat_map<std::string, Signal> _inputs;
	};

	struct System {
		robin_hood::unordered_flat_map<std::string, FlipFlop> ff_nodes;
		robin_hood::unordered_flat_map<std::string, Conjunction> conj_nodes;

		robin_hood::unordered_flat_map<std::string, std::vector<std::string>> connections;

		std::vector<std::string> broadcaster;

		// Assuming rx will be connected to one & module
		std::vector<std::string> rx_parent_inputs;
	};

	System Parse(const std::vector<std::string>& input)
	{
		System s;
		std::string rx_parent = "";
		for (const auto& line : input)
		{
			constexpr auto arrow(" -> ");
			auto arrow_pos = line.find(arrow);

			auto module = line.substr(0, arrow_pos);
			auto connections_str = line.substr(module.size() + 5 - 1);
			
			std::vector<std::string> connections;
			std::stringstream ss(connections_str);
			std::string token;

			while (std::getline(ss, token, ',')) {
				connections.push_back(token);
			}
			for (auto& conn : connections)
			{
				if (conn.front() == ' ')
				{
					conn = conn.substr(1);
				}
				if (conn.back() == ' ')
				{
					conn = conn.substr(0, conn.size() - 1);
				}
			}

			auto module_name = module;
			if (module_name == "broadcaster")
			{
				s.broadcaster = connections;
			}
			else
			{
				module_name = module.substr(1);
			}

			if (module[0] == '%')
			{
				s.ff_nodes[module_name] = {};
			}
			else if (module[0] == '&')
			{
				s.conj_nodes[module_name] = {};
			}

			// Assuming rx will be connected to one module
			if (connections[0] == "rx")
			{
				rx_parent = module_name;
			}

			s.connections[module_name] = connections;
		}

		for (const auto& [src, connections] : s.connections)
		{
			for (const auto& conn : connections) {
				if (conn == rx_parent)
				{
					s.rx_parent_inputs.push_back(src);
				}

				const auto it = std::find_if(s.conj_nodes.begin(), s.conj_nodes.end(),
					[&conn](const auto conj_node) { return conj_node.first == conn; });
				if (it == s.conj_nodes.end()) continue;

				it->second.add_input(src);
			}
		}
		return s;
	}

	using SenderSignalPair = std::pair<std::string, Signal>;
	using PropagationQueue = std::deque<std::pair<SenderSignalPair, std::vector<std::string>>>;

	std::optional<Signal> PinfFFNode(Signal input, const std::string& node_name, FlipFlop& node,
		PropagationQueue& propagation,
		const robin_hood::unordered_flat_map<std::string, std::vector<std::string>>& connections)
	{
		const auto res = node.apply(input);
		if (res.has_value())
		{
			propagation.push_back(
				{ 
					{
						node_name,
						res.value()
					}, 
					connections.at(node_name) 
				}
			);
		}
		return res;
	}

	Signal PinfConjNode(SenderSignalPair input, const std::string& node_name, Conjunction& node,
		PropagationQueue& propagation,
		const robin_hood::unordered_flat_map<std::string, std::vector<std::string>>& connections)
	{
		const auto res = node.apply(input.first, input.second);
		propagation.push_back(
			{
				{
					node_name,
					res
				},
				connections.at(node_name)
			}
		);
		return res;
	}

	void ClickButton(System& system, PropagationQueue propagation,
		std::function<void(const SenderSignalPair&, const std::string&)> prop_callback
	)
	{
		while (!propagation.empty())
		{
			const auto [current_signal, recievers] = propagation.front();
			propagation.pop_front();

			for (const auto& r : recievers)
			{
				prop_callback(current_signal, r);

				//const auto it = std::find_if(system.conj_nodes.begin(), system.conj_nodes.end(),
					//[&r](const auto conj_node) { return conj_node.first == r; });
				const auto it = system.conj_nodes.find(r);
				if (it != system.conj_nodes.end())
				{
					PinfConjNode(current_signal, r, it->second, propagation, system.connections);
					continue;
				}

				//const auto ff_it = std::find_if(system.ff_nodes.begin(), system.ff_nodes.end(),
					//[&r](const auto ff_node) { return ff_node.first == r; });
				const auto ff_it = system.ff_nodes.find(r);
				if (ff_it != system.ff_nodes.end())
				{
					PinfFFNode(current_signal.second, r, ff_it->second, propagation, system.connections);
					continue;
				}
			}
		}
	}
}

namespace AdventOfCode2023 {
	class Day20 : public BaseDayTest
	{
	protected:
		Day20() : BaseDayTest(20) {}
	};

	struct Part {
		int x, m, a, s;
		size_t summ() const {
			return x + m + a + s;
		}
	};

	TEST_F(Day20, FirstStage)
	{
		auto system = Parse(input.GetInputStrByLine());

		size_t lo_pulses = 0;
		size_t hi_pulses = 0;
		for (auto i = 1; i <= 1000; ++i)
		{
			PropagationQueue propagation
				= { {{"broadcaster", Signal::Lo}, system.broadcaster} };
			lo_pulses++;
			ClickButton(system, propagation,
				[&lo_pulses, &hi_pulses](const SenderSignalPair& current_signal, const std::string& reciever) {
				current_signal.second == Signal::Hi ? hi_pulses++ : lo_pulses++;
			});
		}
		std::cout << "Result: " << lo_pulses * hi_pulses << std::endl;
	}

	TEST_F(Day20, SecondStage)
	{
		auto system = Parse(input.GetInputStrByLine());

		std::unordered_map<std::string, std::size_t> periods;
		for (const auto rx_parent_input : system.rx_parent_inputs)
		{
			periods[rx_parent_input] = 0;
		}

		size_t iteration = 0;
		int periods_found = 0;
		bool all_periods_found = false;
		while(!all_periods_found)
		{
			iteration++;
			if (iteration % 500 == 0)
			{
				std::cout << "Working on " << iteration << " iteration" << std::endl;
			}
			PropagationQueue propagation
				= { {{"broadcaster", Signal::Lo}, system.broadcaster} };
			ClickButton(system, propagation,
				[&](const SenderSignalPair& current_signal, const std::string& reciever) {
					// Assuming rx will be connected to one & module
					// all of them must have Hi
					if (current_signal.second != Signal::Hi)
					{
						return;
					}
					if (auto it = periods.find(current_signal.first);
						it != periods.end())
					{
						if (it->second != 0)
						{
							return;
						}

						it->second = iteration; 
						periods_found++;
					}

					if (periods_found == periods.size())
					{
						all_periods_found = true;
					}
				});
		}
		size_t ans = 1;
		for (auto& [name, val] : periods) {
			ans = std::lcm(ans, val);
		}
		std::cout << "Number of clicks: " << ans << std::endl;
	}
}