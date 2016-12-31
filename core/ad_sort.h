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

template <class BidirIt, class Compare>
Void insertionSort(BidirIt first, BidirIt last, Compare comp)
{
    if (first == last) {
        return;
    }
    
    auto i = first, j = first, k = first;
    typename std::iterator_traits<BidirIt>::value_type tmp;

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

template <class BidirIt>
Void insertionSort(BidirIt first, BidirIt last)
{
    insertionSort(first, last, std::less<typename
        std::iterator_traits<BidirIt>::value_type>());
}

template <class RandomIt, class Compare>
Void mergeSort(RandomIt first, RandomIt last, Compare comp)
{
    auto size = last - first;
    using ValueType = typename std::iterator_traits<
        RandomIt>::value_type;

    if (size < 2) {
        return;
    }

    const decltype(size) THRESHOLD = 32;

    for (decltype(size) i = 0; i < size; i += THRESHOLD) {
        insertionSort(first + i, first + std::min(i + THRESHOLD,
            size), comp);
    }

    if (size <= THRESHOLD) {
        return;
    }

    auto ret = std::get_temporary_buffer<ValueType>(size);
    using BuffPtr = decltype(ret.first);
    if (ret.second < size) {
        throw std::bad_alloc();
    }
    auto buff = ret.first;
    decltype(size) num = 0;
    for (decltype(size) i = THRESHOLD; i < size; i <<= 1, ++num) {
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
                    std::move_iterator<RandomIt>(first + j),
                    std::move_iterator<RandomIt>(first + j + i),
                    std::move_iterator<RandomIt>(first + j + i),
                    std::move_iterator<RandomIt>(first +
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

template <class RandomIt>
Void mergeSort(RandomIt first, RandomIt last)
{
    return mergeSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

namespace internal
{

template <class RandomIt, class Compare>
inline RandomIt median(RandomIt first, RandomIt mid,
    RandomIt last, Compare& comp)
{
    if (comp(*first, *mid)) {
        if (comp(*mid, *last)) {
            return mid;
        } else {
            if (comp(*first, *last)) {
                return last;
            } else {
                return first;
            }
        }
    } else {
        if (comp(*first, *last)) {
            return first;
        } else {
            if (comp(*mid, *last)) {
                return last;
            } else {
                return mid;
            }
        }
    }
}

template <class RandomIt, class Compare>
RandomIt partition(RandomIt first, RandomIt last, Compare& comp)
{
    --last;
    auto pivot = *median(first, first + ((last - first) >> 1), last, comp);

    while (1) {
        while (comp(*first, pivot)) {
            ++first;
        }
        while (comp(pivot, *last)) {
            --last;
        }
        if (first >= last) {
            return first;
        }
        std::iter_swap(first, last);
        ++first;
        --last;
    }
}

const std::size_t QUICKSORT_THRESHOLD = 32;

}

template <class RandomIt, class Compare>
Void quickSort(RandomIt first, RandomIt last, Compare comp)
{
    while (last - first > internal::QUICKSORT_THRESHOLD) {
        auto pivot = internal::partition(first, last, comp);

        if (pivot - first < last - pivot) {
            quickSort(first, pivot, comp);
            first = pivot;
        } else {
            quickSort(pivot, last, comp);
            last = pivot;
        }
    }

    insertionSort(first, last, comp);
}

template <class RandomIt>
Void quickSort(RandomIt first, RandomIt last)
{
    quickSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

template <class RandomIt, class Compare>
Void heapSort(RandomIt first, RandomIt last, Compare comp)
{
    std::make_heap(first, last, comp);
    std::sort_heap(first, last, comp);
}

template <class RandomIt>
Void heapSort(RandomIt first, RandomIt last)
{
    heapSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

namespace internal
{

template <class RandomIt, class Compare>
Void introSortImpl(RandomIt first, RandomIt last,
    Compare& comp, Int depth)
{
    while (last - first > internal::QUICKSORT_THRESHOLD) {
        if (!depth) {
            heapSort(first, last, comp);
            return;
        }
        --depth;

        auto pivot = internal::partition(first, last, comp);

        if (pivot - first < last - pivot) {
            introSortImpl(first, pivot, comp, depth);
            first = pivot;
        } else {
            introSortImpl(pivot, last, comp, depth);
            last = pivot;
        }
    }

    insertionSort(first, last, comp);
}

}

template <class RandomIt, class Compare>
Void introSort(RandomIt first, RandomIt last, Compare comp)
{
    internal::introSortImpl(first, last, comp,
        ((Int)std::log2(last - first) << 1));
}

template <class RandomIt>
Void introSort(RandomIt first, RandomIt last)
{
    introSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

}

#endif