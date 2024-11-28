#pragma once
#include "gtest/gtest.h"

#include "../Common/framework.h"

namespace AdventOfCode2023 {
    class BaseDayTest : public testing::Test {
    protected:
        BaseDayTest(int day) {
            // You can do set-up work for each test here.
            input_file_name += std::to_string(day) + ".txt";
        }

        ~BaseDayTest() override {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:
        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).
            ASSERT_TRUE(input.ReadInput("../../OtherDays/Inputs/", input_file_name));
        }

        void TearDown() override {
            // Code here will be called immediately after each test (right
            // before the destructor).
            input.Clear();
        }

        std::string input_file_name = "Day";
        PuzzleInput input;
    };
}