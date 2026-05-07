#include "mock_tape.h"

namespace Testing {

    MockTape::MockTape(std::vector<int32_t> data)
            : data_(std::move(data)), position_(0) {}

    MockTape::MockTape(size_t count)
            : data_(count, 0), position_(0) {}

    int32_t MockTape::read() const {
        return data_[position_];
    }

    void MockTape::write(int32_t value) {
        data_[position_] = value;
    }

    void MockTape::moveRight() {
        position_++;
    }

    void MockTape::moveLeft() {
        position_--;
    }

    void MockTape::rewind() {
        position_ = 0;
    }

    size_t MockTape::size() const {
        return data_.size();
    }

    size_t MockTape::position() const {
        return position_;
    }

    const std::vector<int32_t>& MockTape::data() const {
        return data_;
    }

}
