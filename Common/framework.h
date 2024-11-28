#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <string>
#include <vector>
#include <iostream>

class PuzzleInput {
public:
	PuzzleInput() = default;
	PuzzleInput(std::string folder_name, std::string file_name = "/input.txt");

	bool ReadInput(std::string folder_name, std::string file_name);
	void Clear();

	const std::string& GetInputStr() const;
	const std::vector<std::string>& GetInputStrByLine() const;

private:
	std::string m_input_string;
	std::vector<std::string> m_input_lines;
};


std::vector<int> GetAllNums(std::string::const_iterator begin, std::string::const_iterator end);
std::vector<std::size_t> GetAllNumsULL(std::string::const_iterator begin, std::string::const_iterator end);
std::vector<long long> GetAllNumsLL(std::string::const_iterator begin, std::string::const_iterator end);


std::vector<int> GetAllNums(const std::string& s);
std::vector<std::size_t> GetAllNumsULL(const std::string& s);
std::vector<long long> GetAllNumsLL(const std::string& s);


class BaseDay
{

public:
	BaseDay(std::string day) : input("../Day" + day) { }

	virtual std::string fisrst_stage() = 0;
	virtual std::string second_stage() = 0;

	enum class Stage {
		first,
		second
	};

	void Run(Stage s)
	{
		auto res = (s == Stage::first ? fisrst_stage() : second_stage());
		std::cout << (s == Stage::first ? "First" : "Second") << " stage result: " << res << std::endl;
	}

	void Run()
	{
		Run(Stage::first);
		Run(Stage::second);
	}
protected:
	PuzzleInput input;
};


class DayEXAMPLE final : public BaseDay
{
public:

	std::string fisrst_stage() override
	{
		long long summ = 0;
		for (const auto& line : input.GetInputStrByLine())
		{
			
		}

		return std::to_string(summ);
	}

	std::string second_stage() override
	{
		long long summ = 0;
		for (const auto& line : input.GetInputStrByLine())
		{

		}

		return std::to_string(summ);
	}


	DayEXAMPLE() : BaseDay("EXAMPLE")
	{

	};

};