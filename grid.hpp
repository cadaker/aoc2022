#pragma once

#include <vector>
#include <stdexcept>
#include <iterator>

template<class T, class Tag>
class grid_iterator: public std::iterator<std::random_access_iterator_tag, T> {
public:
    grid_iterator(T* ptr, ssize_t step)
            : _ptr(ptr)
            , _step(step)
    {}

    grid_iterator()
            : _ptr(nullptr)
            , _step(0)
    {}

    grid_iterator(grid_iterator&&)  noexcept = default;
    grid_iterator(grid_iterator const&) = default;
    grid_iterator& operator=(grid_iterator&&)  noexcept = default;
    grid_iterator& operator=(grid_iterator const&) = default;

    T* ptr() const {
        return _ptr;
    }

    typename grid_iterator::reference operator*() const {
        return *ptr();
    }

    typename grid_iterator::pointer operator->() const {
        return ptr();
    }

    grid_iterator& operator++() {
        _ptr += _step;
        return *this;
    }

    grid_iterator operator++(int) {
        auto it = *this;
        ++(*this);
        return it;
    }

    grid_iterator& operator--() {
        _ptr -= _step;
        return *this;
    }

    grid_iterator operator--(int) {
        auto it = *this;
        --(*this);
        return it;
    }

    grid_iterator& operator+=(typename grid_iterator::difference_type n) {
        _ptr += _step*n;
        return *this;
    }

    grid_iterator operator+(typename grid_iterator::difference_type n) const {
        auto it = *this;
        it += n;
        return it;
    }

    grid_iterator& operator-=(typename grid_iterator::difference_type n) {
        _ptr -= _step*n;
        return *this;
    }

    grid_iterator operator-(typename grid_iterator::difference_type n) const {
        auto it = *this;
        it -= n;
        return it;
    }

    typename grid_iterator::difference_type operator-(grid_iterator const& i) const {
        return (i.ptr() - ptr()) / _step;
    }

    typename grid_iterator::reference operator[](typename grid_iterator::difference_type n) const {
        return *((*this) + n);
    }

private:
    T* _ptr;
    ssize_t _step;
};

template<class T, class Tag>
auto operator<=>(grid_iterator<T, Tag> const& i, grid_iterator<T, Tag> const& j) {
    return i.ptr() <=> j.ptr();
}

template<class T, class Tag>
auto operator==(grid_iterator<T, Tag> const& i, grid_iterator<T, Tag> const& j) {
    return i.ptr() == j.ptr();
}

template<class T, class Tag>
auto operator!=(grid_iterator<T, Tag> const& i, grid_iterator<T, Tag> const& j) {
    return i.ptr() != j.ptr();
}

template<class T, class Tag>
auto operator+(typename grid_iterator<T, Tag>::difference_type n, grid_iterator<T, Tag> const& i) {
    return i + n;
}

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

    [[nodiscard]] bool valid_x(long x) const {
        return x >= 0 && x < width();
    }

    [[nodiscard]] bool valid_y(long y) const {
        return y >= 0 && y < height();
    }

    [[nodiscard]] bool valid(long x, long y) const {
        return valid_x(x) && valid_y(y);
    }

    class full_iterator_tag_t {};

    using iterator = grid_iterator<T, full_iterator_tag_t>;
    using const_iterator = grid_iterator<T const, full_iterator_tag_t>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator{_data.begin().base(), 1};
    }

    const_iterator begin() const {
        return const_iterator{_data.begin().base(), 1};
    }

    reverse_iterator rbegin() {
        return reverse_iterator{end()};
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator{end()};
    }

    iterator end() {
        return iterator{_data.end().base(), 1};
    }

    const_iterator end() const {
        return const_iterator{_data.end().base(), 1};
    }

    reverse_iterator rend() {
        return reverse_iterator{begin()};
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator{begin()};
    }

    iterator iter_at(long x, long y) {
        return iterator{&at(x, y), 1};
    }

    const_iterator iter_at(long x, long y) const {
        return const_iterator{&at(x, y), 1};
    }

    class row_iterator_tag_t {};
    using row_iterator = grid_iterator<T, row_iterator_tag_t>;
    using const_row_iterator = grid_iterator<T const, row_iterator_tag_t>;
    using reverse_row_iterator = std::reverse_iterator<row_iterator>;
    using const_reverse_row_iterator = std::reverse_iterator<const_row_iterator>;

    row_iterator row_begin(long y) {
        return row_iterator{&at(0, y), 1};
    }

    const_row_iterator row_begin(long y) const {
        return const_row_iterator{&at(0, y), 1};
    }

    row_iterator row_end(long y) {
        return row_begin(y) + width();
    }

    const_row_iterator row_end(long y) const {
        return row_begin(y) + width();
    }

    reverse_row_iterator row_rbegin(long y) {
        return reverse_row_iterator{row_end(y)};
    }

    const_reverse_row_iterator row_rbegin(long y) const {
        return const_reverse_row_iterator{row_end(y)};
    }

    reverse_row_iterator row_rend(long y) {
        return reverse_row_iterator{row_begin(y)};
    }

    const_reverse_row_iterator row_rend(long y) const {
        return const_reverse_row_iterator{row_begin(y)};
    }

    row_iterator row_iter_at(long x, long y) {
        return row_iterator{&at(x, y), 1};
    }

    const_row_iterator row_iter_at(long x, long y) const {
        return const_row_iterator{&at(x, y), 1};
    }

    reverse_row_iterator row_riter_at(long x, long y) {
        return reverse_row_iterator {row_iter_at(x, y) + 1};
    }

    const_reverse_row_iterator row_riter_at(long x, long y) const {
        return const_reverse_row_iterator {row_iter_at(x, y) + 1};
    }

    class col_iterator_tag_t {};
    using col_iterator = grid_iterator<T, col_iterator_tag_t>;
    using const_col_iterator = grid_iterator<T const, col_iterator_tag_t>;
    using reverse_col_iterator = std::reverse_iterator<col_iterator>;
    using const_reverse_col_iterator = std::reverse_iterator<const_col_iterator>;

    col_iterator col_begin(long x) {
        return col_iterator{&at(x, 0), width()};
    }

    const_col_iterator col_begin(long x) const {
        return const_col_iterator{&at(x, 0), width()};
    }

    col_iterator col_end(long x) {
        return col_begin(x) + height();
    }

    const_col_iterator col_end(long x) const {
        return col_begin(x) + height();
    }

    reverse_col_iterator col_rbegin(long x) {
        return reverse_col_iterator{col_end(x)};
    }

    const_reverse_col_iterator col_rbegin(long x) const {
        return const_reverse_col_iterator{col_end(x)};
    }

    reverse_col_iterator col_rend(long x) {
        return reverse_col_iterator{col_begin(x)};
    }

    const_reverse_col_iterator col_rend(long x) const {
        return const_reverse_col_iterator{col_begin(x)};
    }

    col_iterator col_iter_at(long x, long y) {
        return col_iterator{&at(x, y), width()};
    }

    const_col_iterator col_iter_at(long x, long y) const {
        return const_col_iterator{&at(x, y), width()};
    }

    reverse_col_iterator col_riter_at(long x, long y) {
        return reverse_col_iterator{col_iter_at(x, y) + 1};
    }

    const_reverse_col_iterator col_riter_at(long x, long y) const {
        return const_reverse_col_iterator{col_iter_at(x, y) + 1};
    }

private:
    [[nodiscard]] size_t index(long x, long y) const {
        return y * width() + x;
    }

    size_t _width;
    std::vector<T> _data;
};
