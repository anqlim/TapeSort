#pragma once
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace Configuration {

    struct TapeDelays {
        uint32_t read_delay_ms;
        uint32_t write_delay_ms;
        uint32_t shift_delay_ms;
        uint32_t rewind_delay_ms;
    };

    class Config {
    public:
        explicit Config(const std::string& configPath);
        [[nodiscard]] const TapeDelays& delays() const;
        [[nodiscard]] size_t memoryLimitBytes() const;

    private:
        TapeDelays delays_;
        size_t memory_limit_bytes_;

        void loadFromFile(const std::string& path);
        void applyDefaults();
    };

}
