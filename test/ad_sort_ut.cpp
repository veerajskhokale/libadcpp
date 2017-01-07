#include <forward_list>
#include <list>
#include <vector>
#include <string>
#include <random>
#include <utility>
#include <iterator>
#include <algorithm>

#include "ad_sort.h"
#include "ad_sort_ut.h"
#include "ad_util.h"

namespace ad
{

namespace internal
{

template <class Object, class RandomType>
struct FromRandom
{
    auto operator()(RandomType randomValue)
    {
        return makeObject<Object>(randomValue);
    }
};

template <class RandomType>
struct FromRandom<std::string, RandomType>
{
    auto operator()(RandomType randomValue)
    {
        return std::to_string(randomValue);
    }
};

template <class Object, class RandomType>
inline auto fromRandom(RandomType randomValue)
{
    return FromRandom<Object, RandomType>()(randomValue);
}

template <class InputIt1, class InputIt2, 
    class InputIt3, class Stream>
Void printDiagnostics(InputIt1 inFirst, InputIt1 inLast,
    InputIt2 outFirst, InputIt2 outLast, InputIt3 expectedFirst,
    InputIt3 expectedLast, Stream& strm)
{
    using ValueType = typename InputIt1::value_type;

    strm << "Sort failed";
    strm << "\nInput Size  : " << (inLast - inFirst);
    strm << "\nInput       : ";
    std::copy(inFirst, inLast, 
        std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nOutput      : ";
    std::copy(outFirst, outLast, 
        std::ostream_iterator<ValueType>(strm, " "));
    strm << "\nExpected    : ";
    std::copy(expectedFirst, expectedLast, 
            std::ostream_iterator<ValueType>(strm, " "));
}

template <class Container, class InputIt, 
    class Compare, class Sort, class... Args>
Void verify(InputIt inFirst, InputIt inLast,
    Compare comp, Sort sort, Args&&... args)
{
    using ValueType = typename
        std::iterator_traits<InputIt>::value_type;

    auto cont = makeObject<Container>(inFirst, inLast);
    std::vector<ValueType> expected(inFirst, inLast);

    sort(cont.begin(), cont.end(), std::forward<Args>(args)...);
    std::sort(expected.begin(), expected.end(), comp);

    auto isEqual = std::equal(cont.begin(), 
        cont.end(), expected.begin());

    AD_UT_ASSERT(isEqual, [=](auto& strm) {
        printDiagnostics(inFirst, inLast, cont.begin(),
            cont.end(), expected.begin(), expected.end(), strm);
    });
}

template <class Container, class RandomType, 
    class Compare, class Sort, class... Args>
Void borderTC(Compare comp, Sort sort, Args&&... args)
{
    using ValueType = typename std::decay_t<Container>::value_type;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<std::size_t> sizeud(10, 1000);
    std::uniform_int_distribution<RandomType> ud(
        std::numeric_limits<RandomType>::min(),
        std::numeric_limits<RandomType>::max());

    std::vector<ValueType> in;
    verify<Container>(in.begin(), in.end(), comp, sort,
        std::forward<Args>(args)...);

    in.clear();
    in.push_back(fromRandom<ValueType>(ud(mt)));
    verify<Container>(in.begin(), in.end(), comp, sort,
        std::forward<Args>(args)...);

    in.clear();
    in.push_back(fromRandom<ValueType>(ud(mt)));
    in.push_back(fromRandom<ValueType>(ud(mt)));
    verify<Container>(in.begin(), in.end(), comp, sort,
        std::forward<Args>(args)...);
}

template <class Container, class RandomType, 
    class Compare, class Sort, class... Args>
Void randomTC(Compare comp, Sort sort, Args&&... args)
{
    using ValueType = typename std::decay_t<Container>::value_type;
    const std::size_t NUM_RUNS = 1000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<std::size_t> sizeud(10, 1000);
    std::uniform_int_distribution<RandomType> ud(
        std::numeric_limits<RandomType>::min(),
        std::numeric_limits<RandomType>::max());
    
    for (std::size_t i = 0; i < NUM_RUNS; ++i) {
        std::size_t size = sizeud(mt);
        std::vector<ValueType> in(size);
        std::generate(in.begin(), in.end(), [&ud, &mt]() {
            return fromRandom<ValueType>(ud(mt));
        });
        verify<Container>(in.begin(), in.end(), comp, sort,
            std::forward<Args>(args)...);
    }
}

template <class Container, class RandomType,
    class Compare, class Sort, class... Args>
Void sortedTC(Compare comp, Sort sort, Args&&... args)
{
    using ValueType = typename std::decay_t<Container>::value_type;
    const std::size_t NUM_RUNS = 1000;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<std::size_t> sizeud(10, 1000);
    std::uniform_int_distribution<RandomType> ud(
        std::numeric_limits<RandomType>::min(),
        std::numeric_limits<RandomType>::max());

    for (std::size_t i = 0; i < NUM_RUNS; ++i) {
        std::size_t size = sizeud(mt);
        std::vector<ValueType> in(size);
        std::generate(in.begin(), in.end(), [&ud, &mt]() {
            return fromRandom<ValueType>(ud(mt));
        });
        std::sort(in.begin(), in.end(), comp);
        verify<Container>(in.begin(), in.end(), comp, sort,
            std::forward<Args>(args)...);
        std::sort(in.begin(), in.end(), 
            [comp](const auto& l, const auto& r) {
                return comp(r, l);
            });
        verify<Container>(in.begin(), in.end(), comp, sort,
            std::forward<Args>(args)...);
    }
}

}

AD_UT_STD_COMP_SORT_DEF(stdSort, std::vector, std::sort);
AD_UT_STD_COMP_SORT_DEF(insertionSort, std::vector, insertionSort);
AD_UT_STD_COMP_SORT_DEF(insertionSortList, std::list, insertionSort);
AD_UT_STD_COMP_SORT_DEF(mergeSort, std::vector, mergeSort);
AD_UT_STD_COMP_SORT_DEF(quickSort, std::vector, quickSort);
AD_UT_STD_COMP_SORT_DEF(heapSort, std::vector, heapSort);
AD_UT_STD_COMP_SORT_DEF(introSort, std::vector, introSort);
AD_UT_STD_INT_SORT_DEF(countingSort, std::vector, countingSort);
AD_UT_STD_INT_SORT_DEF(countingSortList, std::list, countingSort);
AD_UT_STD_INT_SORT_DEF(countingSortForwardList, std::forward_list, countingSort);
AD_UT_STD_INT_SORT_DEF(radixSort, std::vector, radixSort);
AD_UT_STD_INT_SORT_DEF(radixSortList, std::list, radixSort);
AD_UT_STD_INT_SORT_DEF(radixSortForwardList, std::forward_list, radixSort);

}