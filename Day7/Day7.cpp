// Day6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <future>
#include <set>
#include <functional>
#include "../Common/framework.h"

class Card {
public:
	enum class type
	{
		none,
		two = 2,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		jack,
		queen,
		king,
		ace
	};

	Card(char c, bool _stage_2)
	{
		switch (c)
		{
		case '2':
			m_type = type::two;
			break;
		case '3':
			m_type = type::three;
			break;
		case '4':
			m_type = type::four;
			break;
		case '5':
			m_type = type::five;
			break;
		case '6':
			m_type = type::six;
			break;
		case '7':
			m_type = type::seven;
			break;
		case '8':
			m_type = type::eight;
			break;
		case '9':
			m_type = type::nine;
			break;
		case 'T':
			m_type = type::ten;
			break;
		case 'J':
			m_type = type::jack;
			break;
		case 'Q':
			m_type = type::queen;
			break;
		case 'K':
			m_type = type::king;
			break;
		case 'A':
			m_type = type::ace;
			break;
		default:
			m_type = type::none;
			break;
		}

		stage_2 = _stage_2;
	}
	Card() : m_type(type::none) {}
	type m_type;

	bool operator < (const Card& other) const
	{
		return this->GetPow() < other.GetPow();
	}

	bool operator > (const Card& other) const
	{
		return this->GetPow() > other.GetPow();
	}

	bool operator == (const Card& other) const
	{
		return this->GetPow() == other.GetPow();
	}

protected:
	bool stage_2 = false;;
	size_t GetPow() const {
		if(stage_2)
			return this->m_type == type::jack ? 1 : static_cast<size_t>(this->m_type);

		return static_cast<size_t>(this->m_type);
	}
};

enum class comb
{
	hight_card,
	pair,
	two_pair,
	three,
	full_house,
	four,
	five
};

struct Round
{
	Round(std::vector<Card> _hand, int _bid) :
		hand(std::move(_hand)),
		bid(_bid)
	{}

	void CalcComb()
	{
		combination = GetHandComb(hand);
	}

	std::vector<Card> hand;
	comb combination;
	int bid;

	friend bool operator > (const Round& f, const Round& other);
	friend bool operator < (const Round& f, const Round& other);


protected:
	virtual bool has_five(const std::vector<Card>& hand)const 
	{

		return std::count(hand.begin(), hand.end(), hand[0]) == 5;

	}
	virtual bool has_four(const std::vector<Card>& hand)const 
	{
		std::set<Card> comb(hand.begin(), hand.end());
		if (comb.size() != 2)
		{
			return false;
		}
		auto num_of_card = std::count(hand.begin(), hand.end(), *(comb.begin()));

		if (num_of_card == 4)
		{
			return true;
		}
		if (num_of_card == 1)
		{
			return true;
		}
		return false;
	}
	virtual bool has_full_house(const std::vector<Card>& hand)const 
	{
		std::set<Card> comb(hand.begin(), hand.end());
		if (comb.size() != 2)
		{
			return false;
		}
		auto num_of_card = std::count(hand.begin(), hand.end(), *(comb.begin()));

		if (num_of_card == 3)
		{
			return true;
		}
		if (num_of_card == 2)
		{
			return true;
		}
		return false;
	}
	virtual bool has_three(const std::vector<Card>& hand)const 
	{
		for (auto it = hand.begin(); it != hand.end(); it++)
		{
			if (std::count(it, hand.end(), *(it)) == 3)
				return true;
		}

		return false;
	}
	virtual bool has_two_pair(const std::vector<Card>& hand)const 
	{
		auto first_pair = hand.end();
		for (auto it = hand.begin(); it != hand.end(); it++)
		{
			if (std::count(it, hand.end(), *(it)) == 2)
			{
				first_pair = it;
				break;
			}
		}

		if (first_pair == hand.end())
		{
			return false;
		}

		for (auto it = first_pair; it != hand.end(); it++)
		{
			if ((*it) == (*first_pair))
			{
				continue;
			}
			if (std::count(it, hand.end(), *(it)) == 2)
			{
				return true;
			}
		}

		return false;
	}
	virtual bool has_pair(const std::vector<Card>& hand)const 
	{
		for (auto it = hand.begin(); it != hand.end(); it++)
		{
			if (std::count(it, hand.end(), *(it)) == 2)
				return true;
		}

		return false;
	}
	Card get_high_card(const std::vector<Card>& hand)const 
	{
		return *std::max_element(hand.begin(), hand.end());
	}

	comb GetHandComb(const std::vector<Card>& hand) const 
	{
		if (has_five(hand))
		{
			return comb::five;
		}
		if (has_four(hand))
		{
			return comb::four;
		}
		if (has_full_house(hand))
		{
			return comb::full_house;
		}
		if (has_three(hand))
		{
			return comb::three;
		}
		if (has_two_pair(hand))
		{
			return comb::two_pair;
		}
		if (has_pair(hand))
		{
			return comb::pair;
		}
		return comb::hight_card;
	}
};

bool operator>(const Round& f, const Round& other)
{
	if (f.combination > other.combination)
	{
		return true;
	}

	if (f.combination < other.combination)
	{
		return false;
	}

	for (int i = 0; i < f.hand.size(); i++)
	{
		if (f.hand[i] > other.hand[i])
		{
			return true;
		}
		if (f.hand[i] < other.hand[i])
		{
			return false;
		}
	}

	return false;
}

bool operator<(const Round& f, const Round& other)
{
	if (f.combination < other.combination)
	{
		return true;
	}

	if (f.combination > other.combination)
	{
		return false;
	}

	for (int i = 0; i < f.hand.size(); i++)
	{
		if (f.hand[i] < other.hand[i])
		{
			return true;
		}
		if (f.hand[i] > other.hand[i])
		{
			return false;
		}
	}

	return false;
}

Card jack_card = Card('J', true);

struct RoundStage2 : Round
{
	RoundStage2(std::vector<Card> _hand, int _bid) : Round(std::move(_hand), std::move(_bid))
	{
		for (const auto& card : hand)
		{
			auto res = card_to_rep.insert(std::pair(card.m_type, 1));
			if (!res.second)
			{
				card_to_rep[card.m_type]++;
			}
		}
	}

	bool has_five(const std::vector<Card>& hand)const override
	{
		if (card_to_rep.size() == 1)
			return true;

		if (card_to_rep.size() > 2)
			return false;

		if (!card_to_rep.contains(Card::type::jack))
		{
			return false;
		}
		return true;
	}
	bool has_four(const std::vector<Card>& hand)const override
	{
		if (!card_to_rep.contains(Card::type::jack))
		{
			return Round::has_four(hand);
		}
		auto jacks = card_to_rep.at(Card::type::jack);
		for (auto it = card_to_rep.cbegin(); it != card_to_rep.cend(); it++)
		{
			if (it->first == Card::type::jack)
				continue;
			if (it->second + jacks == 4)
				return true;
		}
		return false;
	}
	bool has_full_house(const std::vector<Card>& hand)const override
	{
		if (!card_to_rep.contains(Card::type::jack))
		{
			return Round::has_full_house(hand);
		}

		if (card_to_rep.size() > 3)
			return false;

		return true;
	}
	bool has_three(const std::vector<Card>& hand)const override
	{
		if (!card_to_rep.contains(Card::type::jack))
		{
			return Round::has_three(hand);
		}

		auto jacks = card_to_rep.at(Card::type::jack);
		for (auto it = card_to_rep.cbegin(); it != card_to_rep.cend(); it++)
		{
			if (it->first == Card::type::jack)
				continue;
			if (it->second + jacks == 3)
				return true;
		}
		return false;
	}

	bool has_pair(const std::vector<Card>& hand)const override
	{
		if (!card_to_rep.contains(Card::type::jack))
		{
			return Round::has_pair(hand);
		}

		if (card_to_rep.size() == 5)
		{
			return true;
		}
		return false;
	}

	std::map<Card::type, int> card_to_rep;
};

class Day7 final : public BaseDay
{
public: 


	std::string fisrst_stage() override
	{

		std::vector<Round> rounds;

		for (const auto& line : input.GetInputStrByLine())
		{
			std::vector<Card> hand;
			auto pos = line.find(' ');
			for (auto card = 0; card < pos; card++)
			{
				hand.emplace_back(line[card], false);
			}

			int money = std::stoi(line.c_str() + pos + 1);

			rounds.emplace_back(hand, money);
			rounds.back().CalcComb();
		}

		std::sort(rounds.begin(), rounds.end());

		int power = 1;
		int summ = 0;
		for (auto& r : rounds)
		{
			summ += r.bid * power++;
		}
		return std::to_string(summ);
	}

	std::string second_stage() override
	{
		std::vector<RoundStage2> rounds;

		for (const auto& line : input.GetInputStrByLine())
		{
			std::vector<Card> hand;
			auto pos = line.find(' ');
			for (auto card = 0; card < pos; card++)
			{
				hand.emplace_back(line[card], true);
			}

			int money = std::stoi(line.c_str() + pos + 1);

			rounds.emplace_back(hand, money);
			rounds.back().CalcComb();
		}

		std::sort(rounds.begin(), rounds.end());

		int power = 1;
		int summ = 0;
		for (auto& r : rounds)
		{
			summ += r.bid * power++;
		}
		return std::to_string(summ);
	}


	Day7() : BaseDay("7") 
	{

	};

};

int main()
{
	Day7().Run(BaseDay::Stage::second);
}

