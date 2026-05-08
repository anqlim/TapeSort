#include <gtest/gtest.h>
#include "sorter/tape_sorter.h"
#include "mock_tape.h"
#include "config/config.h"

using namespace Tape;
using namespace Sorting;

namespace Testing {

    static Configuration::Config makeConfig(size_t memory_limit_bytes) {
        return Configuration::Config(memory_limit_bytes);
    }

    static bool isSorted(const std::vector<int32_t>& data) {
        for (size_t i = 1; i < data.size(); i++)
            if (data[i] < data[i - 1]) return false;
        return true;
    }

    // Базовые случаи
    TEST(TapeSorterTest, SingleElement) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({42});
        MockTape output(1);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), std::vector<int32_t>({42}));
    }

    TEST(TapeSorterTest, AlreadySorted) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({1, 2, 3, 4, 5});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    TEST(TapeSorterTest, ReverseSorted) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({5, 4, 3, 2, 1});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{1, 2, 3, 4, 5}));
    }

    TEST(TapeSorterTest, AllEqual) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({7, 7, 7, 7});
        MockTape output(4);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{7, 7, 7, 7}));
    }

    TEST(TapeSorterTest, NegativeNumbers) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({-3, -1, -5, -2, -4});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{-5, -4, -3, -2, -1}));
    }

    TEST(TapeSorterTest, MixedNegativePositive) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({3, -1, 0, -5, 2});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{-5, -1, 0, 2, 3}));
    }

    // k < m, несколько чанков
    TEST(TapeSorterTest, MultipleChunksEvenSplit) {
        auto config = makeConfig(2 * sizeof(int32_t));
        TapeFactory factory(config);
        MockTape input({5, 3, 8, 1, 9, 2, 7, 4, 6, 0});
        MockTape output(10);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    TEST(TapeSorterTest, MultipleChunksUnevenSplit) {
        auto config = makeConfig(3 * sizeof(int32_t));
        TapeFactory factory(config);
        MockTape input({9, 1, 5, 3, 7, 2, 8, 4, 6, 0});
        MockTape output(10);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    TEST(TapeSorterTest, ManyChunksMultipleMergePasses) {
        auto config = makeConfig(2 * sizeof(int32_t));
        TapeFactory factory(config);
        std::vector<int32_t> data;
        for (int32_t i = 99; i >= 0; i--) data.push_back(i);
        MockTape input(data);
        MockTape output(100);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    // Краевые случаи
    TEST(TapeSorterTest, TwoElements) {
        auto config = makeConfig(64);
        TapeFactory factory(config);
        MockTape input({2, 1});
        MockTape output(2);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{1, 2}));
    }

    TEST(TapeSorterTest, MaxInt32Values) {
        auto config = makeConfig(2 * sizeof(int32_t));
        TapeFactory factory(config);
        MockTape input({INT32_MAX, INT32_MIN, 0, INT32_MAX - 1, INT32_MIN + 1});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    TEST(TapeSorterTest, ExactlyOneChunk) {
        auto config = makeConfig(5 * sizeof(int32_t));
        TapeFactory factory(config);
        MockTape input({3, 1, 4, 1, 5});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_TRUE(isSorted(output.data()));
    }

    TEST(TapeSorterTest, ChunkSizeOne) {
        auto config = makeConfig(sizeof(int32_t));
        TapeFactory factory(config);
        MockTape input({4, 2, 5, 1, 3});
        MockTape output(5);
        TapeSorter sorter(config, factory);
        sorter.sort(input, output);
        EXPECT_EQ(output.data(), (std::vector<int32_t>{1, 2, 3, 4, 5}));
    }

}