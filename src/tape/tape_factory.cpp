#include "tape_factory.h"

namespace Tape {

    TapeFactory::TapeFactory(const Configuration::Config& config): config_(config) {}

    std::unique_ptr<ITape> TapeFactory::openTape(const std::string& path) const {
        return std::make_unique<FileTape>(path, config_);
    }

    std::unique_ptr<ITape> TapeFactory::createTape(const std::string& path, size_t count) const {
        return std::make_unique<FileTape>(path, count, config_);
    }

}