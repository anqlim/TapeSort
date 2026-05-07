#include "file_tape.h"

namespace Tape {

    FileTape::FileTape(const std::string& file_path, const Configuration::Config& config)
            : file_path_(file_path), size_(0), position_(0), delays_(config.delays())
    {
        file_.open(file_path_, std::ios::in | std::ios::out | std::ios::binary);
        if (!file_.is_open()) {
            throw std::runtime_error("FileTape: cannot open file: " + file_path_);
        }

        file_.seekg(0, std::ios::end);
        const auto byteSize = file_.tellg();
        if (byteSize % sizeof(int32_t) != 0) {
            throw std::runtime_error("FileTape: file size is not a multiple of int32_t: " + file_path_);
        }
        size_ = static_cast<size_t>(byteSize) / sizeof(int32_t);
        file_.seekg(0);
    }

    FileTape::FileTape(const std::string& file_path, size_t elementCount, const Configuration::Config& config)
            : file_path_(file_path), size_(elementCount), position_(0), delays_(config.delays())
    {
        // Создаём или перезаписываем файл нулями
        file_.open(file_path_, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file_.is_open()) {
            throw std::runtime_error("FileTape: cannot create file: " + file_path_);
        }

        const int32_t zero = 0;
        for (size_t i = 0; i < elementCount; ++i) {
            file_.write(reinterpret_cast<const char*>(&zero), sizeof(int32_t));
        }
        file_.seekg(0);
        file_.seekp(0);
    }


    int32_t FileTape::read() const {
        if (position_ >= size_) {
            throw std::runtime_error("FileTape::read — position out of bounds");
        }

        delay(delays_.read_delay_ms);

        seekToCurrent();
        int32_t value = 0;
        file_.read(reinterpret_cast<char*>(&value), sizeof(int32_t));

        if (!file_) {
            throw std::runtime_error("FileTape::read — fstream error at position " + std::to_string(position_));
        }
        return value;
    }

    void FileTape::write(int32_t value) {
        if (position_ >= size_) {
            throw std::runtime_error("FileTape::write — position out of bounds");
        }

        delay(delays_.write_delay_ms);

        seekToCurrent();
        file_.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
        file_.flush(); // сброс буфера

        if (!file_) {
            throw std::runtime_error("FileTape::write — fstream error at position " + std::to_string(position_));
        }
    }

    void FileTape::moveRight() {
        if (position_ >= size_) {
            throw std::runtime_error("FileTape::moveRight — already at end");
        }
        delay(delays_.shift_delay_ms);
        position_++;
    }

    void FileTape::moveLeft() {
        if (position_ == 0) {
            throw std::runtime_error("FileTape::moveLeft — already at beginning");
        }
        delay(delays_.shift_delay_ms);
        position_--;
    }

    void FileTape::rewind() {
        delay(delays_.rewind_delay_ms);
        position_ = 0;
        file_.seekg(0);
        file_.seekp(0);
    }

    size_t FileTape::size() const {
        return size_;
    }

    size_t FileTape::position() const {
        return position_;
    }



    void FileTape::seekToCurrent() const {
        const auto byteOffset = static_cast<std::streamoff>(position_ * sizeof(int32_t));
        file_.seekg(byteOffset);
        file_.seekp(byteOffset);
    }

    void FileTape::delay(uint32_t ms) const {
        if (ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
    }

}