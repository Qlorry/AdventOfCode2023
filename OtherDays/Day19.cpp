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

namespace AdventOfCode2023 {

	class Day19 : public BaseDayTest
	{
	protected:
		Day19() : BaseDayTest(19) {}
	};

	struct Part {
		int x, m, a, s;
		size_t summ() const {
			return x + m + a + s;
		}
	};

	using Result = std::variant<bool, std::string>;

	constexpr int MIN_VALUE = 1;
	constexpr int MAX_VALUE = 4000;
	struct CombinationRange
	{
		std::pair<int, int> x, m, a, s;
		CombinationRange() = default;
		CombinationRange(bool accept)
		{
			x = m = a = s = { MIN_VALUE, accept ? MAX_VALUE : 0 };
		}

		CombinationRange clamp(const CombinationRange& other) const
		{
			const auto clamp_el =
				[](std::pair<int, int> one, std::pair<int, int> another) {
				return std::pair<int, int>{
					std::max(one.first, another.first),
					std::min(one.second, another.second)
				};
			};
			CombinationRange r;

			r.x = clamp_el(x, other.x);
			r.m = clamp_el(m, other.m);
			r.a = clamp_el(a, other.a);
			r.s = clamp_el(s, other.s);
			return r;
		}

		CombinationRange operator!() const
		{
			const auto invert =
				[](std::pair<int, int> prop) {
				if (prop.first == MIN_VALUE && prop.second == MAX_VALUE)
				{
					return prop;
				}

				return std::pair<int, int>{
					prop.first == MIN_VALUE ? prop.second + 1 : MIN_VALUE,
					prop.first == MIN_VALUE ? MAX_VALUE       : prop.first - 1
				};
				};

			CombinationRange r;

			r.x = invert(x);
			r.m = invert(m);
			r.a = invert(a);
			r.s = invert(s);
			return r;
		}

		bool impossible() const 
		{
			return x.first > x.second
				|| m.first > m.second
				|| a.first > a.second
				|| s.first > s.second;
		}

		bool intersects(const CombinationRange& other)
		{
			return (x.first <= other.x.second && x.second >= other.x.first)
				&& (m.first <= other.m.second && m.second >= other.m.first)
				&& (a.first <= other.a.second && a.second >= other.a.first)
				&& (s.first <= other.s.second && s.second >= other.s.first);
		}
	};

	struct IRule {
		virtual bool apply(const Part& to, Result& result) const = 0;
		virtual CombinationRange combinations(Result& outcome) const = 0;
	};

	class AcceptRejectRule : public IRule
	{
	public:
		AcceptRejectRule(bool accept)
		{
			auto_accept = accept;
		}

		bool apply(const Part& to, Result& result) const override
		{
			result = auto_accept;
			return true;
		}

		CombinationRange combinations(Result& outcome) const
		{
			outcome = auto_accept;
			return CombinationRange(auto_accept);
		}
	private:
		bool auto_accept;
	};

	class RedirectRule : public IRule
	{
	public:
		RedirectRule(std::string next_pipeline)
		{
			_next_pipeline = next_pipeline;
		}

		bool apply(const Part& to, Result& result) const override
		{
			result = _next_pipeline;
			return true;
		}

		CombinationRange combinations(Result& outcome) const
		{
			outcome = _next_pipeline;
			return CombinationRange(true);
		}
	private:
		std::string _next_pipeline;
	};

	class GeneralRule : public IRule
	{
	public:
		GeneralRule(char categorie, std::function<bool(int, int)> comp, int comp_val, Result res) :
			_categorie(categorie), _comp(std::move(comp)), 
			_comp_val(comp_val), _res(std::move(res))
		{ }

		bool apply(const Part& to, Result& result) const override
		{
			int value = 0;
			switch (_categorie)
			{
			case 'x':
				value = to.x;
				break;
			case 'm':
				value = to.m;
				break;
			case 'a':
				value = to.a;
				break;
			case 's':
				value = to.s;
				break;
				//default:
				//	ASSERT_TRUE(false);
			}

			if (_comp(value, _comp_val))
			{
				result = _res;
				return true;
			}

			return false;
		}

		CombinationRange combinations(Result& outcome) const
		{
			outcome = _res;
			auto range = CombinationRange(true);
			switch (_categorie)
			{
			case 'x':
				if (_comp(_comp_val + 1, _comp_val))
					range.x.first = _comp_val + 1;
				if (_comp(_comp_val - 1, _comp_val))
					range.x.second = _comp_val - 1;
				break;
			case 'm':
				if (_comp(_comp_val + 1, _comp_val))
					range.m.first = _comp_val + 1;
				if (_comp(_comp_val - 1, _comp_val))
					range.m.second = _comp_val - 1;				
				break;
			case 'a':
				if (_comp(_comp_val + 1, _comp_val))
					range.a.first = _comp_val + 1;
				if (_comp(_comp_val - 1, _comp_val))
					range.a.second = _comp_val - 1;				
				break;
			case 's':
				if (_comp(_comp_val + 1, _comp_val))
					range.s.first = _comp_val + 1;
				if (_comp(_comp_val - 1, _comp_val))
					range.s.second = _comp_val - 1;				
				break;
				//default:
				//	ASSERT_TRUE(false);
			}
			return range;
		}
	private:
		char _categorie;
		std::function<bool(int, int)> _comp;
		int _comp_val;
		Result _res;
	};

	class Pipeline {
	public:
		std::vector<std::unique_ptr<IRule>> actions;
		Result run(const Part& part) const
		{
			Result res = true;
			for (const auto& action : actions)
			{
				if (action->apply(part, res))
				{
					return res;
				}
			}
			return res;
		}

		std::vector<std::pair<Result, CombinationRange>> get_possible_combinations(CombinationRange range)
		{
			std::vector<std::pair<Result, CombinationRange>> combs;

			for (const auto& action : actions)
			{
				Result outcome;
				auto comb = action->combinations(outcome);
				if (!std::holds_alternative<bool>(outcome) 
					|| std::get<bool>(outcome))
				{
					// case for success
					combs.push_back({ outcome, range.clamp(comb) });
				}
				// other case
				range = range.clamp(!comb);

				if(range.impossible())
					return combs;
			}
			return combs;
		}
	};


	std::pair< std::map<std::string, Pipeline>, std::vector<Part>> Parse(const std::vector<std::string>& input)
	{

		std::map<std::string, Pipeline> pipelines;
		std::vector<Part> parts;
		bool objects = false;
		for (const auto& line : input)
		{
			if (line == "") {
				objects = true;
				continue;
			}

			if (!objects)
			{
				const auto open_brace = line.find('{');
				const auto pipeline_name = line.substr(0, open_brace);
				auto& p = pipelines[pipeline_name];

				std::vector<std::string> rules;
				std::stringstream ss(line.substr(open_brace + 1, line.size() - open_brace - 1 - 1));
				std::string token;

				while (std::getline(ss, token, ',')) {
					rules.push_back(token);
				}

				std::set<char> params_to_check;
				for (const auto& action : rules)
				{
					if (const auto colon_pos = action.find(':');
						colon_pos == std::string::npos)
					{
						if (action == "A")
						{
							p.actions.push_back(std::make_unique<AcceptRejectRule>(true));
						}
						else if (action == "R")
						{
							p.actions.push_back(std::make_unique<AcceptRejectRule>(false));
						}
						else
						{
							p.actions.push_back(std::make_unique<RedirectRule>(action));
						}
					}
					else
					{
						auto comp_value_str = action.substr(0, colon_pos);
						auto comp_value = std::stoi(comp_value_str.data() + 2);
						auto action_str = action.substr(colon_pos + 1);
						Result final_res;
						if (action_str == "A")
						{
							final_res = true;
						}
						else if (action_str == "R")
						{
							final_res = false;
						}
						else
						{
							final_res = action_str;
						}
						assert(action[1] == '<' || action[1] == '>');

						const std::function<bool(int, int)> less = [](int part, int comp_value) { return part < comp_value; };
						const std::function<bool(int, int)> more = [](int part, int comp_value) { return part > comp_value; };

						p.actions.push_back(std::make_unique<GeneralRule>(
							action[0],
							action[1] == '<' ? less : more,
							comp_value,
							final_res
						));

					}
				}
			}
			else
			{
				std::vector<std::string> values;
				std::stringstream ss(line.substr(1, line.size() - 1 - 1));
				std::string token;

				while (std::getline(ss, token, ',')) {
					values.push_back(token);
				}

				assert(values.size() == 4);

				parts.emplace_back(
					std::stoi(values[0].data() + 2), //x
					std::stoi(values[1].data() + 2), //m
					std::stoi(values[2].data() + 2), //a
					std::stoi(values[3].data() + 2)  //s
				);
			}
		}

		return { std::move(pipelines), std::move(parts ) };
	}

	TEST_F(Day19, FirstStage)
	{
		auto [pipelines, parts] = Parse(input.GetInputStrByLine());

		const auto in_pipeline = pipelines.find("in");
		ASSERT_NE(in_pipeline, pipelines.end());

		std::vector<Part> accepted_parts;
		for (const auto& p : parts)
		{
			auto current_pipeline = in_pipeline;
			while (current_pipeline != pipelines.end())
			{
				auto res = current_pipeline->second.run(p);
				if (std::holds_alternative<bool>(res))
				{
					current_pipeline = pipelines.end();
					if(std::get<bool>(res))
						accepted_parts.push_back(p);
				}
				else
				{
					current_pipeline = pipelines.find(std::get<std::string>(res));
					ASSERT_NE(current_pipeline, pipelines.end());
				}
			}
		}

		auto res = 0ull;
		for (const auto& p : accepted_parts)
		{
			res += p.summ();
		}

		std::cout << "Parts summ " << res << std::endl;
	}

	TEST_F(Day19, SecondStage)
	{
		auto [pipelines, parts] = Parse(input.GetInputStrByLine());

		std::map<std::string, std::vector<std::pair<int, int>>> ranges;
		const auto in_pipeline = pipelines.find("in");

		auto first_combs = in_pipeline->second.get_possible_combinations(CombinationRange(true));

		std::deque<std::pair<Result, CombinationRange>> to_check;
		std::vector<CombinationRange> success;

		for (auto& [result, combination] : first_combs)
		{
			if (std::holds_alternative<bool>(result))
			{
				success.push_back(combination);
			}
			else
			{
				to_check.push_back({ result, combination });
			}
		}

		while (!to_check.empty())
		{
			const auto [result, combination] = to_check.back();
			to_check.pop_back();

			auto pipeline = pipelines.find(std::get<std::string>(result));
			auto combs = pipeline->second.get_possible_combinations(combination);

			for (auto& [result, combination] : combs)
			{
				if (std::holds_alternative<bool>(result))
				{
					success.push_back(combination);
				}
				else
				{
					to_check.push_back({ result, combination });
				}
			}
		}


		for (auto i = 0; i < success.size(); i++)
		{
			for (auto j = i + 1; j < success.size(); j++)
			{
				if (success[i].intersects(success[j]))
				{
					std::cout << "Has intersection\n";
					std::cout << " first  (x=<" << success[i].x.first << ", " << success[i].x.second << ">"
								     << ", m=<" << success[i].m.first << ", " << success[i].m.second << ">"
								     << ", a=<" << success[i].a.first << ", " << success[i].a.second << ">"
								     << ", s=<" << success[i].s.first << ", " << success[i].s.second << ">\n";
					std::cout << " second (x=<" << success[j].x.first << ", " << success[j].x.second << ">"
						<< ", m=<" << success[j].m.first << ", " << success[j].m.second << ">"
						<< ", a=<" << success[j].a.first << ", " << success[j].a.second << ">"
						<< ", s=<" << success[j].s.first << ", " << success[j].s.second << ">" << std::endl;
				}
			}
		}

		size_t cnt = 0;

		for (const auto& comb : success)
		{
			cnt += size_t(comb.x.second - comb.x.first + 1)
				* size_t(comb.m.second - comb.m.first + 1)
				* size_t(comb.a.second - comb.a.first + 1)
				* size_t(comb.s.second - comb.s.first + 1);
		}

		std::cout << "Combinations " << cnt << std::endl;
		//167409079868000
		//167409079868000
	}
}