#include "config.h"

namespace Configuration {

    Config::Config(const std::string& configPath) {
        applyDefaults();
        loadFromFile(configPath);
    }

    Config::Config(size_t memory_limit_bytes) : memory_limit_bytes_(memory_limit_bytes) {
        applyDefaults();
    }

    const TapeDelays& Config::delays() const {
        return delays_;
    }

    size_t Config::memoryLimitBytes() const {
        return memory_limit_bytes_;
    }

    void Config::applyDefaults() {
        delays_.read_delay_ms = 0;
        delays_.write_delay_ms = 0;
        delays_.shift_delay_ms = 0;
        delays_.rewind_delay_ms = 0;
        memory_limit_bytes_ = 32 * 1024 * 1024;
    }

    void Config::loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return;
        }

        try {
            nlohmann::json j;
            file >> j;

            if (j.contains("read_delay_ms")) delays_.read_delay_ms = j["read_delay_ms"];
            if (j.contains("write_delay_ms")) delays_.write_delay_ms = j["write_delay_ms"];
            if (j.contains("shift_delay_ms")) delays_.shift_delay_ms = j["shift_delay_ms"];
            if (j.contains("rewind_delay_ms")) delays_.rewind_delay_ms = j["rewind_delay_ms"];
            if (j.contains("memory_limit_mb")) {
                memory_limit_bytes_ = static_cast<size_t>(j["memory_limit_mb"]) * 1024 * 1024;
            }
        }
        catch (const nlohmann::json::exception&) {
            applyDefaults();
        }
    }

}