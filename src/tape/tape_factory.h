#pragma once
#include "interface.h"
#include "file_tape.h"
#include "../config/config.h"
#include <memory>
#include <string>

namespace Tape {

    class TapeFactory {
    public:
        explicit TapeFactory(const Configuration::Config& config);
        [[nodiscard]] std::unique_ptr<ITape> openTape(const std::string& path) const;
        [[nodiscard]] std::unique_ptr<ITape> createTape(const std::string& path, size_t count) const;

    private:
        const Configuration::Config& config_;
    };

}
