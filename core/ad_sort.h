#ifndef AD_CORE_SORT_H_
#define AD_CORE_SORT_H_

#include <iterator>
#include <memory>
#include <algorithm>
#include <utility>

#include "ad_types.h"
#include "ad_util.h"

namespace ad
{

template <class BidirItT, class Compare>
Void insertionSort(BidirItT first, BidirItT last, Compare comp)
{
    if (first == last) {
        return;
    }
    
    auto i = first, j = first, k = first;
    typename std::iterator_traits<BidirItT>::value_type tmp;

    for (++i; i != last; ++i) {
        if (comp(*i, *first)) {
            j = i;
            tmp = std::move(*i);
            std::move_backward(first, i, ++j);
            *first = std::move(tmp);
        } else {
            j = k = i;
            --j;
            tmp = std::move(*i);
            while (comp(tmp, *j)) {
                *k = std::move(*j);
                --j;
                --k;
            }
            *k = std::move(tmp);
        }
    }
}

template <class BidirItT>
Void insertionSort(BidirItT first, BidirItT last)
{
    insertionSort(first, last, std::less<typename
        std::iterator_traits<BidirItT>::value_type>());
}

template <class RandomItT, class Compare>
Void mergeSort(RandomItT first, RandomItT last, Compare comp)
{
    auto size = last - first;
    using ValueType = typename std::iterator_traits<
        RandomItT>::value_type;

    if (size < 2) {
        return;
    }

    auto ret = std::get_temporary_buffer<ValueType>(size);
    using BuffPtr = decltype(ret.first);
    if (ret.second < size) {
        throw std::bad_alloc();
    }
    auto buff = ret.first;
    decltype(size) num = 0;
    for (decltype(size) i = 1; i < size; i <<= 1, ++num) {
        if (num & 1) {
            decltype(size) j = 0;
            for (; j < size - i; j += (i << 1)) {
                std::merge(
                    std::move_iterator<BuffPtr>(buff + j),
                    std::move_iterator<BuffPtr>(buff + j + i),
                    std::move_iterator<BuffPtr>(buff + j + i),
                    std::move_iterator<BuffPtr>(buff +
                        std::min(size, j + (i << 1))),
                    first + j,
                    comp
                );
            }
            if (j < size) {
                std::move(buff + j, buff + size, first + j);
            }
        } else {
            decltype(size) j = 0;
            for (; j < size - i; j += (i << 1)) {
                std::merge(
                    std::move_iterator<RandomItT>(first + j),
                    std::move_iterator<RandomItT>(first + j + i),
                    std::move_iterator<RandomItT>(first + j + i),
                    std::move_iterator<RandomItT>(first +
                        std::min(size, j + (i << 1))),
                    buff + j,
                    comp
                );
            }
            if (j < size) {
                std::move(first + j, first + size, buff + j);
            }
        }
    }

    if (num & 1) {
        std::move(buff, buff + size, first);
    }

    std::return_temporary_buffer(buff);
}

template <class RandomItT>
Void mergeSort(RandomItT first, RandomItT last)
{
    return mergeSort(first, last, std::less<typename
        std::iterator_traits<RandomItT>::value_type>());
}

}

#endif