#pragma once
#include "tape/interface.h"
#include <vector>
#include <stdexcept>

namespace Testing {

class MockTape : public Tape::ITape {
    public:
        explicit MockTape(std::initializer_list<int32_t> data);
        explicit MockTape(std::vector<int32_t> data);
        explicit MockTape(size_t count);

        int32_t read() const override;
        void write(int32_t value) override;
        void moveRight() override;
        void moveLeft() override;
        void rewind() override;
        size_t size() const override;
        size_t position() const override;

        [[nodiscard]] const std::vector<int32_t>& data() const;

    private:
        std::vector<int32_t> data_;
        size_t position_;
    };

}