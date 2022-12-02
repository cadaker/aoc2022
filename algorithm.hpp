#pragma once

#include <iterator>
#include <numeric>

template<std::forward_iterator Iterator, class Acc, class Mapping, class Op = std::plus<>>
requires std::invocable<Mapping, std::iter_value_t<Iterator>>
         && std::invocable<Op, Acc&&, std::result_of_t<Mapping(std::iter_value_t<Iterator>)>&&>
Acc accumulate_map(Iterator first, Iterator last, Acc acc, Mapping&& f, Op&& op = {}) {
    return std::accumulate(first, last, acc, [&](Acc&& acc, auto&& value) {
        return op(acc, f(value));
    });
}
