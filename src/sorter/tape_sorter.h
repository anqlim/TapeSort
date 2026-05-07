#pragma once
#include "tape/tape_factory.h"
#include <filesystem>
#include <queue>
#include <algorithm>

namespace Sorting {

    class TapeSorter {
    public:
        TapeSorter(const Configuration::Config& config, const Tape::TapeFactory& factory);
        void sort(Tape::ITape& input, Tape::ITape& output);

    private:
        const Configuration::Config& config_;
        const Tape::TapeFactory& factory_;

        size_t split(Tape::ITape& input);
        void merge(Tape::ITape& output, size_t chunk_count, size_t pass = 0);
    };

}
