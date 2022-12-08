#pragma once

#include <vector>
#include <stdexcept>

template<class T>
class grid {
public:
    grid(size_t width, size_t height)
            : _width(width)
            , _data(width * height)
    {
    }

    template<std::input_iterator Iter>
    grid(size_t width, Iter begin, Iter end)
            : _width(width)
            , _data(begin, end)
    {
    }

    grid(grid&&) noexcept = default;
    grid(grid const&) = default;
    grid& operator=(grid&&) noexcept = default;
    grid& operator=(grid const&) = default;

    T const& at(long x, long y) const {
        if (!valid(x,y)) {
            throw std::out_of_range("out of range: " + std::to_string(x) + "," + std::to_string(y));
        }
        return _data.at(index(x, y));
    }

    T & at(long x, long y) {
        if (!valid(x,y)) {
            throw std::out_of_range("out of range: " + std::to_string(x) + "," + std::to_string(y));
        }
        return _data.at(index(x, y));
    }

    [[nodiscard]] long width() const {
        return static_cast<long>(_width);
    }

    [[nodiscard]] long height() const {
        if (_data.empty()) {
            return 0;
        } else {
            return static_cast<long>(_data.size()) / width();
        }
    }

    [[nodiscard]] bool valid(long x, long y) const {
        return x >= 0 && y >= 0 && x < width() && y < height();
    }

    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    iterator begin() {
        return _data.begin();
    }

    const_iterator begin() const {
        return _data.begin();
    }

    iterator end() {
        return _data.end();
    }

    const_iterator end() const {
        return _data.end();
    }

private:
    [[nodiscard]] size_t index(long x, long y) const {
        return y * width() + x;
    }

    size_t _width;
    std::vector<T> _data;
};