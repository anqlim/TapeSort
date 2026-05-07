#pragma once
#include <cstdint>

namespace Tape {

    class ITape {
    public:
        virtual ~ITape() = default;
        [[nodiscard]] virtual int32_t read() const = 0;
        virtual void write(int32_t value) = 0;
        virtual void moveRight() = 0;
        virtual void moveLeft() = 0;
        virtual void rewind() = 0;
        [[nodiscard]] virtual size_t size() const = 0;
        [[nodiscard]] virtual size_t position() const = 0;
    };

}
