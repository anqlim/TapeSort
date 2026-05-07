#pragma once
#include "interface.h"
#include "../config/config.h"
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <fstream>
#include <cstdint>

namespace Tape {

    class FileTape : public ITape {
    public:
        FileTape(const std::string& filePath, const Configuration::Config& config);
        // Создать новую ленту заданной длины (файл перезаписывается нулями)
        FileTape(const std::string& filePath, size_t elementCount, const Configuration::Config& config);
        ~FileTape() override = default;
        FileTape(const FileTape&) = delete; // запрет копирования
        FileTape& operator=(const FileTape&) = delete; // запрет копирования
        FileTape(FileTape&&) = default;
        FileTape& operator=(FileTape&&) = default;

        int32_t read() const override;
        void write(int32_t value) override;
        void moveRight() override;
        void moveLeft() override;
        void rewind() override;
        size_t size() const override;
        size_t position() const override;

    private:
        std::string file_path_;
        mutable std::fstream file_;
        size_t size_;
        size_t position_;
        Configuration::TapeDelays delays_;

        void seekToCurrent() const;
        void delay(uint32_t ms) const;
    };

}
