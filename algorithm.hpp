#pragma once

#include <iterator>
#include <vector>
#include <numeric>
#include <algorithm>

template<std::forward_iterator Iterator, class Acc, class Mapping, class Op = std::plus<>>
requires std::invocable<Mapping, std::iter_value_t<Iterator>>
         && std::invocable<Op, Acc&&, std::result_of_t<Mapping(std::iter_value_t<Iterator>)>&&>
Acc accumulate_map(Iterator first, Iterator last, Acc acc, Mapping&& f, Op&& op = {}) {
    return std::accumulate(first, last, acc, [&](Acc&& acc, auto&& value) {
        return op(acc, f(value));
    });
}

template<class T, class F>
requires std::invocable<F, T const&>
std::vector<std::result_of_t<F(T const&)>> map(std::vector<T> const& v, F&& f) {
    std::vector<std::result_of_t<F(T const&)>> ret;
    ret.reserve(v.size());
    std::transform(v.begin(), v.end(), std::back_inserter(ret), f);
    return ret;
}
