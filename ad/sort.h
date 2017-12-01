/*
 * Copyright 2017 Veeraj S Khokale All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    sort.h
 * @brief   Sorting algorithms
 *
 * This file contains implementations for a number of different sorting
 * algorithms. Currently all the sorting algorithms are single threaded.
 * The supported algorithms are
 *
 * 1. Insertion sort
 * 2. Merge sort
 * 3. Quick sort
 * 4. Heap sort
 * 5. Intro sort
 * 6. Counting sort
 * 7. Radix sort
 */

#ifndef AD_CORE_SORT_H_
#define AD_CORE_SORT_H_

#include <iterator>
#include <memory>
#include <algorithm>
#include <utility>

#include "ad/types.h"
#include "ad/utility.h"

namespace ad
{

/**
 * @brief   Sort a range using insertion sort
 *
 * @tparam      BidirIt     Iterator type, should satisfy Bidirectional iterator
 * @tparam      Compare     Comparsion function object type
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 * @comp[in]    comp        Comparision function object
 *
 * Uses the insertion sort algorithm to sort the input range [first, last)
 * using the comparision function object comp to compare the elements.
 *
 * Iterator requirements    : Bidirectional
 * Best complexity          : O(n), O(1) sorted input
 * Expected complexity      : O(n^2), O(1)
 * Worst complexity         : O(n^2), O(1)
 * Stable                   : Yes
 *
 * Insertion sort can be used to sort any container which provides iterators
 * satisfying bidirectional iterator concept. This makes it possible to sort
 * lists as well with insertion sort. Insertion sort works best for nearly
 * sorted input ranges for which it has very good performance. It also works
 * well for small input sizes, and is used by other sorting algorithms like
 * merge sort and quick sort when the input size becomes small.
 */
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

/**
 * @brief   Sort a range using insertion sort
 *
 * @tparam      BidirIt     Iterator type, should satisfy Bidirectional iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * Same as insertionSort() except that it uses operator < instead of the
 * comparision object. For more details see insertionSort()
 */
template <class BidirIt>
Void insertionSort(BidirIt first, BidirIt last)
{
    insertionSort(first, last, std::less<typename
        std::iterator_traits<BidirIt>::value_type>());
}

/**
 * @brief   Functor wrapper for insertionSort()
 */
struct InsertionSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        insertionSort(std::forward<Args>(args)...);
    }
};

/**
 * @brief   Sort a range using merge sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 * @tparam      Compare     Comparsion function object type
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 * @comp[in]    comp        Comparision function object
 *
 * @exception   std::bad_alloc  When memory allocation fails
 *
 * Uses the merge sort algorithm to sort the input range [first, last)
 * using the comparision function object comp to compare the elements.
 *
 * Iterator requirements    : Bidirectional
 * Complexity               : O(nlog(n)), O(n)
 * Stable                   : Yes
 *
 * Merge sort is a stable sorting algorithm which provides good performance
 * on any input types at the cost of extra space. To increase performance
 * it switches to insertionSort() when the input size becomes small.
 */
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

    TmpBuff<ValueType> tmpBuff(size);
    tmpBuff.init();
    using BuffIter = typename TmpBuff<ValueType>::Iterator;
    if (tmpBuff.capacity() < size) {
        throw std::bad_alloc();
    }
    auto buff = tmpBuff.begin();

    decltype(size) num = 0;
    for (decltype(size) i = THRESHOLD; i < size; i <<= 1, ++num) {
        if (num & 1) {
            decltype(size) j = 0;
            for (; j < size - i; j += (i << 1)) {
                std::merge(
                    std::move_iterator<BuffIter>(buff + j),
                    std::move_iterator<BuffIter>(buff + j + i),
                    std::move_iterator<BuffIter>(buff + j + i),
                    std::move_iterator<BuffIter>(buff +
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
}

/**
 * @brief   Sort a range using merge sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * @exception   std::bad_alloc  When memory allocation fails
 *
 * Same as mergeSort() except that it uses operator < instead of the
 * comparision object. For more details see mergeSort()
 */
template <class RandomIt>
Void mergeSort(RandomIt first, RandomIt last)
{
    return mergeSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

/**
 * @brief   Functor wrapper for mergeSort()
 */
struct MergeSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        mergeSort(std::forward<Args>(args)...);
    }
};

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

/**
 * @brief   Sort a range using quick sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 * @tparam      Compare     Comparsion function object type
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 * @comp[in]    comp        Comparision function object
 *
 * Uses the quick sort algorithm to sort the input range [first, last)
 * using the comparision function object comp to compare the elements.
 *
 * Iterator requirements    : Random Access
 * Best complexity          : O(nlog(n)), O(1) sorted input
 * Expected complexity      : O(nlog(n)), O(1)
 * Worst complexity         : O(n^2), O(1) median of 3 killer input
 * Stable                   : No
 *
 * Quick sort is a fast sorting algorithm which works well for most inputs.
 * This implementation of quick sort uses median of 3 partitioning and
 * therefore a median of 3 killer input (see wikipedia) can elicit the
 * worst case behaviour. To increase performance it switches to
 * insertionSort() when the input size becomes small.
 */
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

/**
 * @brief   Sort a range using quick sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * Same as quickSort() except that it uses operator < instead of the
 * comparision object. For more details see quickSort()
 */
template <class RandomIt>
Void quickSort(RandomIt first, RandomIt last)
{
    quickSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

/**
 * @brief   Functor wrapper for quickSort()
 */
struct QuickSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        quickSort(std::forward<Args>(args)...);
    }
};

/**
 * @brief   Sort a range using heap sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 * @tparam      Compare     Comparsion function object type
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 * @comp[in]    comp        Comparision function object
 *
 * Uses the heap sort algorithm to sort the input range [first, last)
 * using the comparision function object comp to compare the elements.
 *
 * Iterator requirements    : Random Access
 * Best complexity          : O(nlog(n)), O(1)
 * Expected complexity      : O(nlog(n)), O(1)
 * Worst complexity         : O(nlog(n)), O(1)
 * Stable                   : No
 *
 * Heap sort builds an in-place heap to sort the elements in the range.
 * Though it has good asymtotic bounds it is slower than other nlog(n)
 * comparision sorts because of the bigger hidden constants in the
 * asymtotic times.
 */
template <class RandomIt, class Compare>
Void heapSort(RandomIt first, RandomIt last, Compare comp)
{
    std::make_heap(first, last, comp);
    std::sort_heap(first, last, comp);
}

/**
 * @brief   Sort a range using heap sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * Same as heapSort() except that it uses operator < instead of the
 * comparision object. For more details see heapSort()
 */
template <class RandomIt>
Void heapSort(RandomIt first, RandomIt last)
{
    heapSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

/**
 * @brief   Functor wrapper for heapSort()
 */
struct HeapSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        heapSort(std::forward<Args>(args)...);
    }
};

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

/**
 * @brief   Sort a range using intro sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 * @tparam      Compare     Comparsion function object type
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 * @comp[in]    comp        Comparision function object
 *
 * Uses the intro sort algorithm to sort the input range [first, last)
 * using the comparision function object comp to compare the elements.
 *
 * Iterator requirements    : Random Access
 * Best complexity          : O(nlog(n)), O(1)
 * Expected complexity      : O(nlog(n)), O(1)
 * Worst complexity         : O(nlog(n)), O(1)
 * Stable                   : No
 *
 * Intro sort is a fast sorting algorithm which combines quick sort and
 * heap sort. This ensures that the worst case time of quick sort is
 * never realized since intro sort automatically switches to heap sort
 * if quick sort runs very slowly for that input. To increase performance
 * further it switches to insertionSort() when the input size becomes
 * small. This is the sorting algorithm of choice for general sorting
 * use cases.
 */
template <class RandomIt, class Compare>
Void introSort(RandomIt first, RandomIt last, Compare comp)
{
    internal::introSortImpl(first, last, comp,
        ((Int)std::log2(last - first) << 1));
}

/**
 * @brief   Sort a range using quick sort
 *
 * @tparam      RandomIt    Iterator type, should satisfy Random Access iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * Same as introSort() except that it uses operator < instead of the
 * comparision object. For more details see introSort()
 */
template <class RandomIt>
Void introSort(RandomIt first, RandomIt last)
{
    introSort(first, last, std::less<typename
        std::iterator_traits<RandomIt>::value_type>());
}

/**
 * @brief   Functor wrapper for introSort()
 */
struct IntroSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        introSort(std::forward<Args>(args)...);
    }
};

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

    TmpBuff<typename std::iterator_traits<ForwardIt>::value_type> outBuff(size);
    outBuff.init();
    if (outBuff.capacity() < size) {
        throw std::bad_alloc();
    }
    auto out = outBuff.begin();

    TmpBuff<Size> countBuff(range);
    countBuff.init(0);
    if (countBuff.capacity() < range) {
        throw std::bad_alloc();
    }
    auto count = countBuff.begin();

    internal::countingSortImpl(first, last, count, range, out,
        [key, min](const auto& val) {
            return key(*val) - min;
        });

    std::move(out, out + size, first);
}

/**
 * @brief   Sort a range using counting sort
 *
 * @tparam      ForwardIt   Iterator type, should satisfy Forward iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * @exception   std::bad_alloc  When memory allocation fails
 *
 * Uses counting sort to sort the range [first, last) where each element
 * in the range is assumed to be an integral type. If R is the difference
 * between the largest element and the smallest element in the range
 * [first, last) then R should fit in the integral type. This places
 * restrictions on signed integral inputs where this may not always be the
 * case. Further since this algorithm allocates R * sizeof(value type)
 * bytes the allocation is likely to fail for large R. If the input range
 * is not integral but each element can be identified with an integral
 * key then counting sort can still be used. See countingSort() with key
 * for details.
 *
 * Iterator requirements    : Forward
 * Best complexity          : O(n + R), O(R)
 * Expected complexity      : O(n + R), O(R)
 * Worst complexity         : O(n + R), O(R)
 * Stable                   : Yes
 *
 * Counting sort can be used for most containers like vectors, lists,
 * forward_lists, sets, maps etc. since it only requires forward iterators.
 * It should be used when the input is known to be integral and R is
 * comparable to the size of the input. In this case the run time is
 * linear.
 */
template <class ForwardIt>
Void countingSort(ForwardIt first, ForwardIt last)
{
    if (first == last) {
        return;
    }

    auto min = *std::min_element(first, last);
    auto max = *std::max_element(first, last);

    auto range = (PtrDiff)(max - min + 1);

    TmpBuff<Size> countBuff(range);
    countBuff.init(0);
    if (countBuff.capacity() < range) {
        throw std::bad_alloc();
    }
    auto count = countBuff.begin();

    for (auto i = first; i != last; ++i) {
        *i -= min;
    }
    internal::countingSortImpl(first, last, count, range);
    for (auto i = first; i != last; ++i) {
        *i += min;
    }
}

/**
 * @brief   Functor wrapper for countingSort()
 */
struct CountingSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        countingSort(std::forward<Args>(args)...);
    }
};

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

    TmpBuff<typename std::iterator_traits<ForwardIt>::value_type> outBuff(size);
    outBuff.init();
    if (outBuff.capacity() < size) {
        throw std::bad_alloc();
    }
    auto out = outBuff.begin();

    TmpBuff<Size> countBuff(range);
    countBuff.init(0);
    if (countBuff.capacity() < range) {
        throw std::bad_alloc();
    }
    auto count = countBuff.begin();

    for (Size i = 0, j = 0; i < numPasses; ++i, j += bitsPerPass) {
        countingSortImpl(first, last, count, range, out,
            [mask, j](const auto& val) {
                return (val >> j) & mask;
            });

        std::move(out, out + size, first);
    }
}

}

/**
 * @brief   Sort a range using radix sort
 *
 * @tparam      ForwardIt   Iterator type, should satisfy Forward iterator
 *
 * @param[in]   first       Iterator to the first element in the range
 * @param[in]   last        Iterator to the last element in the range
 *
 * @exception   std::bad_alloc  When memory allocation fails
 *
 * Uses radix sort to sort the range [first, last) where each element
 * in the range is assumed to be an integral type. If R is the difference
 * between the largest element and the smallest element in the range
 * [first, last) then R should fit in the integral type. This places
 * restrictions on signed integral inputs where this may not always be the
 * case.
 *
 * Iterator requirements    : Forward
 * Best complexity          : O(nlog(R)), O(log(R))
 * Expected complexity      : O(nlog(R)), O(log(R))
 * Worst complexity         : O(nlog(R)), O(log(R))
 * Stable                   : Yes
 *
 * Radix sort can be used for most containers like vectors, lists,
 * forward_lists, sets, maps etc. since it only requires forward iterators.
 * It should be used when the input is known to be integral and R is
 * comparable to the size of the input.
 */
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

/**
 * @brief   Functor wrapper for radixSort()
 */
struct RadixSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        radixSort(std::forward<Args>(args)...);
    }
};

/**
 * @brief   Functor wrapper for std::sort()
 */
struct StdSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        std::sort(std::forward<Args>(args)...);
    }
};

/**
 * @brief   Functor wrapper for std::stable_sort()
 */
struct StdStableSort
{
    template <class... Args>
    Void operator()(Args&&... args)
    {
        std::stable_sort(std::forward<Args>(args)...);
    }
};

}

#endif