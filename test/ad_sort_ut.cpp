#include <list>
#include <vector>
#include <random>
#include <utility>
#include <iterator>
#include <algorithm>

#include "ad_sort.h"
#include "ad_sort_ut.h"
#include "ad_util.h"

namespace ad
{

template <class InputIt1T, class InputIt2T, class Compare, class StreamT>
Void getDiagnostics(InputIt1T inFirst, InputIt1T inLast,
    InputIt2T outFirst, InputIt2T outLast, Compare&& comp, StreamT& strm)
{
    using ValueType = typename InputIt1T::value_type;
    std::vector<ValueType> sorted(inFirst, inLast);
    std::sort(sorted.begin(), sorted.end(), std::forward<Compare>(comp));
    strm << "Sort failed\nInput       : ";
    std::copy(inFirst, inLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nOutput      : ";
    std::copy(outFirst, outLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nExpected    : ";
    std::copy(sorted.begin(), sorted.end(), std::ostream_iterator<ValueType>(strm, " "));
}

template <class InputIt1T, class InputIt2T, class StreamT>
Void getDiagnostics(InputIt1T inFirst, InputIt1T inLast,
    InputIt2T outFirst, InputIt2T outLast, StreamT& strm)
{
    using ValueType = typename InputIt1T::value_type;
    std::vector<ValueType> sorted(inFirst, inLast);
    std::sort(sorted.begin(), sorted.end());
    strm << "Sort failed\nInput       : ";
    std::copy(inFirst, inLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nOutput      : ";
    std::copy(outFirst, outLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nExpected    : ";
    std::copy(sorted.begin(), sorted.end(), std::ostream_iterator<ValueType>(strm, " "));
}

template <class ContainerT, class Sort, class Compare>
Void borderTC(Sort sort, Compare&& comp)
{
    using ValueType = typename ContainerT::value_type;

    auto cont = makeObject<ContainerT>();
    sort(cont.begin(), cont.end(), std::forward<Compare>(comp));

    cont.clear();
    cont.push_back(std::random_device()());
    sort(cont.begin(), cont.end(), std::forward<Compare>(comp));

    cont.clear();
    cont.push_back(std::random_device()());
    cont.push_back(std::random_device()());
    std::vector<ValueType> inCopy(cont.begin(), cont.end());
    sort(cont.begin(), cont.end(), std::forward<Compare>(comp));

    auto isSorted = std::is_sorted(cont.begin(), cont.end(), 
        std::forward<Compare>(comp));

    AD_UT_ASSERT(isSorted, [=](auto& strm) {
        getDiagnostics(inCopy.begin(), inCopy.end(),
            cont.begin(), cont.end(), comp, strm);
    });
}

template <class ContainerT, class Sort>
Void borderTC(Sort sort)
{
    using ValueType = typename ContainerT::value_type;

    auto cont = makeObject<ContainerT>();
    sort(cont.begin(), cont.end());

    cont.clear();
    cont.push_back(std::random_device()());
    sort(cont.begin(), cont.end());

    cont.clear();
    cont.push_back(std::random_device()());
    cont.push_back(std::random_device()());
    std::vector<ValueType> inCopy(cont.begin(), cont.end());
    sort(cont.begin(), cont.end());

    auto isSorted = std::is_sorted(cont.begin(), cont.end());

    AD_UT_ASSERT(isSorted, [=](auto& strm) {
        getDiagnostics(inCopy.begin(), inCopy.end(),
            cont.begin(), cont.end(), strm);
    });
}

template <class ContainerT, class Sort, class Compare>
Void randomTC(Sort sort, Compare&& comp)
{
    using ValueType = typename ContainerT::value_type;
    const Int SIZE_1 = 100;
    const Int NUM_RUNS = 10000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<ValueType> ud(
        std::numeric_limits<ValueType>::min(),
        std::numeric_limits<ValueType>::max());
    std::vector<ValueType> inCopy(SIZE_1);
    for (Int i = 0; i < NUM_RUNS; ++i) {
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        auto cont = makeObject<ContainerT>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end(), std::forward<Compare>(comp));
        auto isSorted = std::is_sorted(cont.begin(), cont.end(),
            std::forward<Compare>(comp));
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), comp, strm);
        });
    }
}

template <class ContainerT, class Sort>
Void randomTC(Sort sort)
{
    using ValueType = typename ContainerT::value_type;
    const Int SIZE_1 = 100;
    const Int NUM_RUNS = 10000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<ValueType> ud(
        std::numeric_limits<ValueType>::min(),
        std::numeric_limits<ValueType>::max());
    std::vector<ValueType> inCopy(SIZE_1);
    for (Int i = 0; i < NUM_RUNS; ++i) {
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        auto cont = makeObject<ContainerT>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end());
        auto isSorted = std::is_sorted(cont.begin(), cont.end());
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), strm);
        });
    }
}

template <class ContainerT, class Sort, class Compare>
Void sortedTC(Sort sort, Compare&& comp)
{
    using ValueType = typename ContainerT::value_type;
    const Int SIZE_1 = 100;
    const Int NUM_RUNS = 100;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<ValueType> ud;
    std::vector<ValueType> inCopy(SIZE_1);
    for (Int i = 0; i < NUM_RUNS; ++i) {
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        std::sort(inCopy.begin(), inCopy.end(), std::forward<Compare>(comp));
        auto cont = makeObject<ContainerT>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end(), std::forward<Compare>(comp));
        auto isSorted = std::is_sorted(cont.begin(), cont.end(),
            std::forward<Compare>(comp));
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), comp, strm);
        });

        std::sort(inCopy.begin(), inCopy.end(), 
            [comp](const auto& a, const auto& b) {
                return comp(b, a);
            });
        std::copy(inCopy.begin(), inCopy.end(), cont.begin());
        sort(cont.begin(), cont.end(), std::forward<Compare>(comp));
        isSorted = std::is_sorted(cont.begin(), cont.end(),
            std::forward<Compare>(comp));
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), comp, strm);
        });
    }
}

template <class ContainerT, class Sort>
Void sortedTC(Sort sort)
{
    using ValueType = typename ContainerT::value_type;
    const Int SIZE_1 = 100;
    const Int NUM_RUNS = 100;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<ValueType> ud;
    std::vector<ValueType> inCopy(SIZE_1);
    for (Int i = 0; i < NUM_RUNS; ++i) {
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        std::sort(inCopy.begin(), inCopy.end());
        auto cont = makeObject<ContainerT>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end());
        auto isSorted = std::is_sorted(cont.begin(), cont.end());
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), strm);
        });
    }
}

AD_UT_DEFINE(insertionSortBorderTC) 
{
    borderTC<std::vector<Int>>(insertionSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    borderTC<std::vector<Int>>(insertionSort<
        std::vector<Int>::iterator>);

    borderTC<std::list<Int>>(insertionSort<
        std::list<Int>::iterator, std::less<Int>>, std::less<Int>());
    borderTC<std::list<Int>>(insertionSort<
        std::list<Int>::iterator>);
}

AD_UT_DEFINE(insertionSortRandomTC) 
{
    randomTC<std::vector<Int64>>(insertionSort<
        std::vector<Int64>::iterator, std::less<Int64>>, std::less<Int64>());
    randomTC<std::vector<Int64>>(insertionSort<
        std::vector<Int64>::iterator>);

    randomTC<std::list<Int64>>(insertionSort<
        std::list<Int64>::iterator, std::less<Int64>>, std::less<Int64>());
    randomTC<std::list<Int64>>(insertionSort<
        std::list<Int64>::iterator>);
}

AD_UT_DEFINE(insertionSortSortedTC) 
{
    sortedTC<std::vector<Int>>(insertionSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    sortedTC<std::vector<Int>>(insertionSort<
            std::vector<Int>::iterator>);

    sortedTC<std::list<Int>>(insertionSort<
        std::list<Int>::iterator, std::less<Int>>, std::less<Int>());
    sortedTC<std::list<Int>>(insertionSort<
            std::list<Int>::iterator>);
}

}