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

const PtrDiff QUICKSORT_THRESHOLD = 32;

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

namespace internal
{

template <class ForwardIt, class CountIt, class OutIt, class Key>
Void countingSortImpl(ForwardIt first, ForwardIt last,
    CountIt count, Size range, OutIt out, Key key)
{
    std::fill(count, count + range, 0);
    for (auto i = first; i != last; ++i) {
        ++count[key(*i)];
    }

    Size tmp, tot = 0;
    for (Size i = 0; i < range; ++i) {
        tmp = count[i];
        count[i] = tot;
        tot += tmp;
    }

    for (auto i = first; i != last; ++i) {
        tmp = key(*i);
        out[count[tmp]++] = std::move(*i);
    }
}

template <class ForwardIt, class CountIt>
Void countingSortImpl(ForwardIt first, ForwardIt last,
    CountIt count, Size range)
{
    std::fill(count, count + range, 0);
    for (auto i = first; i != last; ++i) {
        ++count[*i];
    }

    for (Size i = 0; i < range; ++i) {
        while (count[i]--) {
            *first = i;
            ++first;
        }
    }
}

}

template <class ForwardIt, class Key>
Void countingSort(ForwardIt first, ForwardIt last, Key key)
{
    if (first == last) {
        return;
    }

    auto min = *std::min_element(first, last,
        [key](const auto& l, const auto& r) {
            return key(*l) < key(*r);
        });

    auto max = *std::max_element(first, last,
        [key](const auto& l, const auto& r) {
            return key(*l) < key(*r);
        });

    Size size = (Size)std::distance(first, last);
    auto range = (Size)(max - min + 1);

    auto ret1 = std::get_temporary_buffer<typename
        std::iterator_traits<ForwardIt>::value_type>(size);
    if (ret1.second < size) {
        throw std::bad_alloc();
    }
    auto out = ret1.first;

    auto ret2 = std::get_temporary_buffer<Size>(range);
    if (ret2.second < range) {
        throw std::bad_alloc();
    }
    auto count = ret2.first;

    internal::countingSortImpl(first, last, count, range, out,
        [key, min](const auto& val) {
            return key(*val) - min;
        });

    std::move(out, out + size, first);

    std::return_temporary_buffer(out);
    std::return_temporary_buffer(count);
}

template <class ForwardIt>
Void countingSort(ForwardIt first, ForwardIt last)
{
    if (first == last) {
        return;
    }

    auto min = *std::min_element(first, last);
    auto max = *std::max_element(first, last);

    auto range = (PtrDiff)(max - min + 1);

    auto ret = std::get_temporary_buffer<Size>(range);
    if (ret.second < range) {
        throw std::bad_alloc();
    }
    auto count = ret.first;

    for (auto i = first; i != last; ++i) {
        *i -= min;
    }
    internal::countingSortImpl(first, last, count, range);
    for (auto i = first; i != last; ++i) {
        *i += min;
    }

    std::return_temporary_buffer(count);
}

template <class ForwardIt, class Sort, class PassCompare>
Void radixSort(ForwardIt first, ForwardIt last,
    Size numPasses, Sort sort, PassCompare comp)
{
    for (Size i = 0; i < numPasses; ++i) {
        sort(first, last, [&](const auto& l, const auto& r) {
            return comp(l, r, i);
        });
    }
}

template <class ForwardIt, class Key>
Void radixSort(ForwardIt first, ForwardIt last,
    Size numPasses, Key key)
{
    for (Size i = 0; i < numPasses; ++i) {
        countingSort(first, last, [&](const auto& val) {
            return key(val, i);
        });
    }
}

namespace internal
{

template <class ForwardIt>
Void radixSortImpl(ForwardIt first, ForwardIt last,
    Size numBits, Size bitsPerPass)
{
    Size numPasses = numBits / bitsPerPass +
        (numBits % bitsPerPass != 0);

    PtrDiff range = ((PtrDiff)(1)) << (bitsPerPass);
    Size mask = range - 1;
    PtrDiff size = std::distance(first, last);

    auto ret1 = std::get_temporary_buffer<typename
        std::iterator_traits<ForwardIt>::value_type>(size);
    if (ret1.second < size) {
        throw std::bad_alloc();
    }
    auto out = ret1.first;

    auto ret2 = std::get_temporary_buffer<Size>(range);
    if (ret2.second < range) {
        throw std::bad_alloc();
    }
    auto count = ret2.first;

    for (Size i = 0, j = 0; i < numPasses; ++i, j += bitsPerPass) {
        countingSortImpl(first, last, count, range, out,
            [mask, j](const auto& val) {
                return (val >> j) & mask;
            });

        std::move(out, out + size, first);
    }

    std::return_temporary_buffer(count);
    std::return_temporary_buffer(out);
}

}

template <class ForwardIt>
Void radixSort(ForwardIt first, ForwardIt last)
{
    if (first == last) {
        return;
    }

    auto min = *std::min_element(first, last);

    for (auto i = first; i != last; ++i) {
        *i -= min;
    }

    auto max = *std::max_element(first, last);
    Size numBits = (max ? std::log2(max) : 0) + 1;
    Size bitsPerPass = std::log2(numBits) + 1;

    internal::radixSortImpl(first, last, numBits, bitsPerPass);

    for (auto i = first; i != last; ++i) {
        *i += min;
    }
}

}

#endif