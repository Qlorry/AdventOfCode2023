// Day5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <future>
#include "../Common/framework.h"


struct Section {
    std::size_t src_start;
    std::size_t dst_start;
    std::size_t len;
};

std::vector< Section > fill_map(std::vector < std::string>::const_iterator begin, std::vector < std::string>::const_iterator end)
{
    std::vector< Section > res;
    for (auto it = begin; it != end; it++)
    {
        auto vals = GetAllNumsULL(it->cbegin(), it->cend());
        if (vals.empty())
        {
            continue;
        }
        res.push_back({ static_cast<std::size_t>(vals[1]), static_cast<std::size_t>(vals[0]), static_cast<std::size_t>(vals[2]) });
    }
    return res;
}

std::pair<std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator> find_section(std::string section_name,  std::vector < std::string>::const_iterator begin, std::vector < std::string>::const_iterator end)
{
    auto section_start = begin;
    section_start = std::find_if(section_start, end, [&section_name](const auto& str)
        {
            return str.find(section_name) != std::string::npos;
        });

    auto section_end = std::find(section_start, end, "");
    if (section_start == end)
    {
        std::cout << "Panic!\n";
    }

    return { section_start, section_end };
}

std::size_t find_dest(std::vector<Section>& sections, std::size_t val)
{
    for (auto& s : sections)
    {
        if (val >= s.src_start && val < s.src_start + s.len)
        {
            return s.dst_start + (val - s.src_start);
        }
    }
    return val;
}

void first_stage()
{
    std::vector<Section> seed_to_soil;
    std::vector<Section> soil_to_fertilizer;
    std::vector<Section> fertilizer_to_water;
    std::vector<Section> water_to_light;
    std::vector<Section> light_to_temperature;
    std::vector<Section> temperature_to_humidity;
    std::vector<Section> humidity_to_location;

    PuzzleInput inp("../Day5");
    const auto& input = inp.GetInputStrByLine();

    auto section = find_section("seed-to-soil", std::next(input.begin()), input.end());
    seed_to_soil = fill_map(section.first, section.second);

    section = find_section("soil-to-fertilizer", section.second, input.end());
    soil_to_fertilizer = fill_map(section.first, section.second);

    section = find_section("fertilizer-to-water", section.second, input.end());
    fertilizer_to_water = fill_map(section.first, section.second);

    section = find_section("water-to-light", section.second, input.end());
    water_to_light = fill_map(section.first, section.second);

    section = find_section("light-to-temperature", section.second, input.end());
    light_to_temperature = fill_map(section.first, section.second);

    section = find_section("temperature-to-humidity", section.second, input.end());
    temperature_to_humidity = fill_map(section.first, section.second);

    section = find_section("humidity-to-location", section.second, input.end());
    humidity_to_location = fill_map(section.first, section.second);

    auto seeds = GetAllNumsULL(input[0].cbegin(), input[0].cend());
    auto min_loc = std::numeric_limits<std::size_t>::max();
    for (auto seed : seeds)
    {
        auto soil = find_dest(seed_to_soil, seed);
        auto fertilizer = find_dest(soil_to_fertilizer, soil);
        auto water = find_dest(fertilizer_to_water, fertilizer);
        auto light = find_dest(water_to_light, water);
        auto temperature = find_dest(light_to_temperature, light);
        auto humidity = find_dest(temperature_to_humidity, temperature);
        auto loc = find_dest(humidity_to_location, humidity);

        min_loc = std::min(min_loc, loc);
    }

    std::cout << "Result: " << min_loc;
}


void second_stage()
{
    std::vector<Section> seed_to_soil;
    std::vector<Section> soil_to_fertilizer;
    std::vector<Section> fertilizer_to_water;
    std::vector<Section> water_to_light;
    std::vector<Section> light_to_temperature;
    std::vector<Section> temperature_to_humidity;
    std::vector<Section> humidity_to_location;

    PuzzleInput inp("../Day5");
    const auto& input = inp.GetInputStrByLine();

    auto section = find_section("seed-to-soil", std::next(input.begin()), input.end());
    seed_to_soil = fill_map(section.first, section.second);

    section = find_section("soil-to-fertilizer", section.second, input.end());
    soil_to_fertilizer = fill_map(section.first, section.second);

    section = find_section("fertilizer-to-water", section.second, input.end());
    fertilizer_to_water = fill_map(section.first, section.second);

    section = find_section("water-to-light", section.second, input.end());
    water_to_light = fill_map(section.first, section.second);

    section = find_section("light-to-temperature", section.second, input.end());
    light_to_temperature = fill_map(section.first, section.second);

    section = find_section("temperature-to-humidity", section.second, input.end());
    temperature_to_humidity = fill_map(section.first, section.second);

    section = find_section("humidity-to-location", section.second, input.end());
    humidity_to_location = fill_map(section.first, section.second);

    auto seed_ranges_raw = GetAllNumsULL(input[0].cbegin(), input[0].cend());
    std::vector<std::pair<std::size_t, std::size_t>> seed_ranges;
    for (int i = 0; i < seed_ranges_raw.size(); i += 2)
    {
        seed_ranges.emplace_back(seed_ranges_raw[i], seed_ranges_raw[i + 1]);
    }

    std::vector<std::future<std::size_t>> th_res;
    for (auto [start_seed, len] : seed_ranges)
    {
        th_res.push_back(std::async(std::launch::async,
            [&seed_to_soil, &soil_to_fertilizer, &fertilizer_to_water, &water_to_light, &light_to_temperature, &temperature_to_humidity, &humidity_to_location]
            (size_t start_seed, size_t len) {
                auto min_loc_local = std::numeric_limits<std::size_t>::max();

                for (auto seed = start_seed; seed < start_seed + len; seed++)
                {
                    auto soil = find_dest(seed_to_soil, seed);
                    auto fertilizer = find_dest(soil_to_fertilizer, soil);
                    auto water = find_dest(fertilizer_to_water, fertilizer);
                    auto light = find_dest(water_to_light, water);
                    auto temperature = find_dest(light_to_temperature, light);
                    auto humidity = find_dest(temperature_to_humidity, temperature);
                    auto loc = find_dest(humidity_to_location, humidity);

                    min_loc_local = std::min(min_loc_local, loc);
                }
                return min_loc_local;
            },
            start_seed, len)
        );
    }

    auto min_loc = std::numeric_limits<std::size_t>::max();
    for (auto& res : th_res)
    {
        min_loc = std::min(min_loc, res.get());
    }

    std::cout << "Result: " << min_loc;
}


int main()
{
    second_stage();
}
