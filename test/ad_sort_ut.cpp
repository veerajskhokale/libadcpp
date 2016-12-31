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

template <class InputIt1, class InputIt2, class Compare, class Stream>
Void getDiagnostics(InputIt1 inFirst, InputIt1 inLast,
    InputIt2 outFirst, InputIt2 outLast, Compare&& comp, Stream& strm)
{
    using ValueType = typename InputIt1::value_type;
    std::vector<ValueType> sorted(inFirst, inLast);
    std::sort(sorted.begin(), sorted.end(), std::forward<Compare>(comp));
    strm << "Sort failed\nInput       : ";
    std::copy(inFirst, inLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nOutput      : ";
    std::copy(outFirst, outLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nExpected    : ";
    std::copy(sorted.begin(), sorted.end(), std::ostream_iterator<ValueType>(strm, " "));
}

template <class InputIt1, class InputIt2, class Stream>
Void getDiagnostics(InputIt1 inFirst, InputIt1 inLast,
    InputIt2 outFirst, InputIt2 outLast, Stream& strm)
{
    using ValueType = typename InputIt1::value_type;
    std::vector<ValueType> sorted(inFirst, inLast);
    std::sort(sorted.begin(), sorted.end());
    strm << "Sort failed\nInput       : ";
    std::copy(inFirst, inLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nOutput      : ";
    std::copy(outFirst, outLast, std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nExpected    : ";
    std::copy(sorted.begin(), sorted.end(), std::ostream_iterator<ValueType>(strm, " "));
}

template <class Container, class Sort, class Compare>
Void borderTC(Sort sort, Compare&& comp)
{
    using ValueType = typename Container::value_type;

    auto cont = makeObject<Container>();
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

template <class Container, class Sort>
Void borderTC(Sort sort)
{
    using ValueType = typename Container::value_type;

    auto cont = makeObject<Container>();
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

template <class Container, class Sort, class Compare>
Void randomTC(Sort sort, Compare&& comp)
{
    using ValueType = typename Container::value_type;
    const Int NUM_RUNS = 10000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<Int> sizeud(10, 1000);
    std::uniform_int_distribution<ValueType> ud(
        std::numeric_limits<ValueType>::min(),
        std::numeric_limits<ValueType>::max());
    for (Int i = 0; i < NUM_RUNS; ++i) {
        Int size = sizeud(mt);
        std::vector<ValueType> inCopy(size);
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        auto cont = makeObject<Container>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end(), std::forward<Compare>(comp));
        auto isSorted = std::is_sorted(cont.begin(), cont.end(),
            std::forward<Compare>(comp));
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), comp, strm);
        });
    }
}

template <class Container, class Sort>
Void randomTC(Sort sort)
{
    using ValueType = typename Container::value_type;
    const Int NUM_RUNS = 10000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<Int> sizeud(10, 1000);
    std::uniform_int_distribution<ValueType> ud(
        std::numeric_limits<ValueType>::min(),
        std::numeric_limits<ValueType>::max());
    for (Int i = 0; i < NUM_RUNS; ++i) {
        Int size = sizeud(mt);
        std::vector<ValueType> inCopy(size);
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        auto cont = makeObject<Container>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end());
        auto isSorted = std::is_sorted(cont.begin(), cont.end());
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), strm);
        });
    }
}

template <class Container, class Sort, class Compare>
Void sortedTC(Sort sort, Compare&& comp)
{
    using ValueType = typename Container::value_type;
    const Int NUM_RUNS = 100;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<Int> sizeud(10, 1000);
    std::uniform_int_distribution<ValueType> ud;
    for (Int i = 0; i < NUM_RUNS; ++i) {
        Int size = sizeud(mt);
        std::vector<ValueType> inCopy(size);
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        std::sort(inCopy.begin(), inCopy.end(), std::forward<Compare>(comp));
        auto cont = makeObject<Container>(inCopy.begin(), inCopy.end());
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

template <class Container, class Sort>
Void sortedTC(Sort sort)
{
    using ValueType = typename Container::value_type;
    const Int NUM_RUNS = 100;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<Int> sizeud(10, 1000);
    std::uniform_int_distribution<ValueType> ud;
    for (Int i = 0; i < NUM_RUNS; ++i) {
        Int size = sizeud(mt);
        std::vector<ValueType> inCopy(size);
        std::generate(inCopy.begin(), inCopy.end(), [&ud, &mt]() {
            return ud(mt);
        });
        std::sort(inCopy.begin(), inCopy.end());
        auto cont = makeObject<Container>(inCopy.begin(), inCopy.end());
        sort(cont.begin(), cont.end());
        auto isSorted = std::is_sorted(cont.begin(), cont.end());
        AD_UT_ASSERT(isSorted, [=](auto& strm) {
            getDiagnostics(inCopy.begin(), inCopy.end(),
                cont.begin(), cont.end(), strm);
        });
    }
}

AD_UT_DEFINE(stdSortBorderTC)
{
    borderTC<std::vector<Int>>(std::sort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    borderTC<std::vector<Int>>(std::sort<
        std::vector<Int>::iterator>);
}

AD_UT_DEFINE(stdSortRandomTC)
{
    randomTC<std::vector<Int64>>(std::sort<
        std::vector<Int64>::iterator, std::less<Int64>>, std::less<Int64>());
    randomTC<std::vector<Int64>>(std::sort<
        std::vector<Int64>::iterator>);
}

AD_UT_DEFINE(stdSortSortedTC)
{
    sortedTC<std::vector<Int>>(std::sort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    sortedTC<std::vector<Int>>(std::sort<
            std::vector<Int>::iterator>);
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

AD_UT_DEFINE(mergeSortBorderTC)
{
    borderTC<std::vector<Int>>(mergeSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    borderTC<std::vector<Int>>(mergeSort<
        std::vector<Int>::iterator>);
}

AD_UT_DEFINE(mergeSortRandomTC)
{
    randomTC<std::vector<Int64>>(mergeSort<
        std::vector<Int64>::iterator, std::less<Int64>>, std::less<Int64>());
    randomTC<std::vector<Int64>>(mergeSort<
        std::vector<Int64>::iterator>);
}

AD_UT_DEFINE(mergeSortSortedTC)
{
    sortedTC<std::vector<Int>>(mergeSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    sortedTC<std::vector<Int>>(mergeSort<
            std::vector<Int>::iterator>);
}

AD_UT_DEFINE(quickSortBorderTC)
{
    borderTC<std::vector<Int>>(quickSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    borderTC<std::vector<Int>>(quickSort<
        std::vector<Int>::iterator>);
}

AD_UT_DEFINE(quickSortRandomTC)
{
    randomTC<std::vector<Int64>>(quickSort<
        std::vector<Int64>::iterator, std::less<Int64>>, std::less<Int64>());
    randomTC<std::vector<Int64>>(quickSort<
        std::vector<Int64>::iterator>);
}

AD_UT_DEFINE(quickSortSortedTC)
{
    sortedTC<std::vector<Int>>(quickSort<
        std::vector<Int>::iterator, std::less<Int>>, std::less<Int>());
    sortedTC<std::vector<Int>>(quickSort<
            std::vector<Int>::iterator>);
}

}