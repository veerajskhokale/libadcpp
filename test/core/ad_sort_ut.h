#include <forward_list>
#include <list>
#include <vector>
#include <string>
#include <random>
#include <utility>
#include <iterator>
#include <algorithm>

#include "ad_sort.h"
#include "ad_ut.h"
#include "ad_util.h"

namespace ad
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

    strm << " Sort failed";
    strm << "\n Input Size  : " << (inLast - inFirst);
    strm << "\n Input       : ";
    std::copy(inFirst, inLast,
        std::ostream_iterator<ValueType>(strm, " "));
    strm << "\n Output      : ";
    std::copy(outFirst, outLast,
        std::ostream_iterator<ValueType>(strm, " "));
    strm << "\n Expected    : ";
    std::copy(expectedFirst, expectedLast,
            std::ostream_iterator<ValueType>(strm, " "));
}

template <class Container, class InputIt,
    class Compare, class Sort, class... Args>
Void verify(InputIt inFirst, InputIt inLast,
    Compare comp, Bool isStable, Sort sort, Args&&... args)
{
    using ValueType = typename
        std::iterator_traits<InputIt>::value_type;

    auto cont = makeObject<Container>(inFirst, inLast);
    std::vector<ValueType> expected(inFirst, inLast);

    sort(cont.begin(), cont.end(), std::forward<Args>(args)...);
    if (isStable) {
        std::stable_sort(expected.begin(), expected.end(), comp);
    } else {
        std::sort(expected.begin(), expected.end(), comp);
    }

    auto isEqual = std::equal(cont.begin(),
        cont.end(), expected.begin());

    AD_UT_ASSERT(isEqual, [=](auto& strm) {
        printDiagnostics(inFirst, inLast, cont.begin(),
            cont.end(), expected.begin(), expected.end(), strm);
    });
}

template <class Container, class RandomType>
struct BorderTC
{
    template <class Compare, class Sort, class... Args>
    Void operator()(Compare comp, Bool isStable, Sort sort, Args&&... args)
    {
        using ValueType = typename std::decay_t<Container>::value_type;

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<RandomType> ud(
            std::numeric_limits<RandomType>::min(),
            std::numeric_limits<RandomType>::max());

        std::vector<ValueType> in;
        verify<Container>(in.begin(), in.end(), comp, isStable, sort,
            std::forward<Args>(args)...);

        in.clear();
        in.push_back(fromRandom<ValueType>(ud(mt)));
        verify<Container>(in.begin(), in.end(), comp, isStable, sort,
            std::forward<Args>(args)...);

        in.clear();
        in.push_back(fromRandom<ValueType>(ud(mt)));
        in.push_back(fromRandom<ValueType>(ud(mt)));
        verify<Container>(in.begin(), in.end(), comp, isStable, sort,
            std::forward<Args>(args)...);
    }
};

template <class Container, class RandomType>
struct RandomTC
{
    template <class Compare, class Sort, class... Args>
    Void operator()(Compare comp, Bool isStable, Sort sort, Args&&... args)
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
            verify<Container>(in.begin(), in.end(), comp, isStable, sort,
                std::forward<Args>(args)...);
        }
    }
};

template <class Container, class RandomType>
struct SortedTC
{
    template <class Compare, class Sort, class... Args>
    Void operator()(Compare comp, Bool isStable, Sort sort, Args&&... args)
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
            verify<Container>(in.begin(), in.end(), comp, isStable, sort,
                std::forward<Args>(args)...);
            std::sort(in.begin(), in.end(),
                [comp](const auto& l, const auto& r) {
                    return comp(r, l);
                });
            verify<Container>(in.begin(), in.end(), comp, isStable, sort,
                std::forward<Args>(args)...);
        }
    }
};

enum class SortType { COMP_SORT, INT_SORT };
enum class SortIterType { RANDOM_ITER, BIDIR_ITER, FORWARD_ITER };

template <template <typename, typename> class TC, class CompSort,
    template <typename, typename> class Container, class ValType,
    class RandomType, template <typename> class Compare, Bool isStable>
struct CompSortWithComp
    : public UnitTest
{
    Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            Compare<ValType>(), isStable, CompSort(), Compare<ValType>()
        );
    }
};

template <template <typename, typename> class TC, class CompSort,
    template <typename, typename> class Container,
    class ValType, class RandomType, Bool isStable>
struct CompSortWithoutComp
    : public UnitTest
{
    Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            std::less<ValType>(), isStable, CompSort()
        );
    }
};

template <template <typename, typename> class TC, class IntSort,
    template <typename, typename> class Container,
    class ValType, class RandomType, Bool isStable>
struct IntegerSort
    : public UnitTest
{
    Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            std::less<ValType>(), isStable, IntSort()
        );
    }
};

template <template <typename, typename> class TC, class Sort,
    template <typename, typename> class Container,
    SortType sortType, Bool isStable>
struct AddSortTypes
{
};

template <template <typename, typename> class TC, class IntSort,
    template <typename, typename> class Container, Bool isStable>
struct AddSortTypes<TC, IntSort, Container, SortType::INT_SORT, isStable>
    : public UnitTest
{
    template <class IntType>
    Void addHelper(UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, IntSort, Container, IntType, IntType, isStable>>(name);
    }

    Void operator()()
    {
        UTRunner utRunner;
        addHelper<Int8>(utRunner, "Int8");
        addHelper<Int16>(utRunner, "Int16");
        addHelper<Int32>(utRunner, "Int32");
        addHelper<Int64>(utRunner, "Int64");
        addHelper<Uint8>(utRunner, "Uint8");
        addHelper<Uint16>(utRunner, "Uint16");
        addHelper<Uint32>(utRunner, "Uint32");
        addHelper<Uint64>(utRunner, "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC,
    template <typename, typename> class Container, Bool isStable>
struct AddSortTypes<TC, CountingSort, Container, SortType::INT_SORT, isStable>
    : public UnitTest
{
    template <class IntType, class RandomType>
    Void addHelper(UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, CountingSort, Container, IntType, RandomType, isStable>>(name);
    }

    Void operator()()
    {
        UTRunner utRunner;
        addHelper<Int16, Int8>(utRunner, "Int16-8");
        addHelper<Int32, Int16>(utRunner, "Int32-16");
        addHelper<Int64, Int16>(utRunner, "Int64-16");
        addHelper<Uint8, Uint8>(utRunner, "Uint8");
        addHelper<Uint16, Uint16>(utRunner, "Uint16");
        addHelper<Uint32, Uint16>(utRunner, "Uint32-16");
        addHelper<Uint64, Uint16>(utRunner, "Uint64-16");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC,
    template <typename, typename> class Container, Bool isStable>
struct AddSortTypes<TC, RadixSort, Container, SortType::INT_SORT, isStable>
    : public UnitTest
{
    template <class IntType, class RandomType>
    Void addHelper(UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, RadixSort, Container, IntType, RandomType, isStable>>(name);
    }

    Void operator()()
    {
        UTRunner utRunner;
        addHelper<Int16, Int8>(utRunner, "Int16-8");
        addHelper<Int32, Int16>(utRunner, "Int32-16");
        addHelper<Int64, Int16>(utRunner, "Int64-16");
        addHelper<Uint8, Uint8>(utRunner, "Uint8");
        addHelper<Uint16, Uint16>(utRunner, "Uint16");
        addHelper<Uint32, Uint32>(utRunner, "Uint32");
        addHelper<Uint64, Uint64>(utRunner, "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC, class CompSort,
    template <typename, typename> class Container, Bool isStable>
struct AddSortTypes<TC, CompSort, Container, SortType::COMP_SORT, isStable>
    : public UnitTest
{
    template <class IntType>
    Void addHelper(UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<CompSortWithComp<TC, CompSort, Container, IntType, IntType, std::less, isStable>>(name + "Less");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, IntType, IntType, std::greater, isStable>>(name + "Greater");
    }

    Void operator()()
    {
        UTRunner utRunner;
        const std::string COMP_SORT_NAME = "CompSort";
        utRunner.add<AddSortTypes<TC, CompSort, Container, SortType::INT_SORT, isStable>>("CompSort");
        utRunner.add<CompSortWithoutComp<TC, CompSort, Container, std::string, Int64, isStable>>(COMP_SORT_NAME + "StringI64");
        utRunner.add<CompSortWithoutComp<TC, CompSort, Container, std::string, Uint64, isStable>>(COMP_SORT_NAME + "StringU64");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, std::string, Int64, std::less, isStable>>(COMP_SORT_NAME + "StringLess");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, std::string, Uint64, std::greater, isStable>>(COMP_SORT_NAME + "StringGreater");
        addHelper<Int8>(utRunner, COMP_SORT_NAME + "Int8");
        addHelper<Int16>(utRunner, COMP_SORT_NAME + "Int16");
        addHelper<Int32>(utRunner, COMP_SORT_NAME + "Int32");
        addHelper<Int64>(utRunner, COMP_SORT_NAME + "Int64");
        addHelper<Uint8>(utRunner, COMP_SORT_NAME + "Uint8");
        addHelper<Uint16>(utRunner, COMP_SORT_NAME + "Uint16");
        addHelper<Uint32>(utRunner, COMP_SORT_NAME + "Uint32");
        addHelper<Uint64>(utRunner, COMP_SORT_NAME + "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, template <typename, typename> class Container,
    SortType sortType, Bool isStable>
struct AddSortTCs
    : public UnitTest
{
    Void operator()()
    {
        UTRunner utRunner;
        utRunner.add<AddSortTypes<BorderTC, Sort, Container, sortType, isStable>>("BorderTC");
        utRunner.add<AddSortTypes<RandomTC, Sort, Container, sortType, isStable>>("RandomTC");
        utRunner.add<AddSortTypes<SortedTC, Sort, Container, sortType, isStable>>("SortedTC");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType,
    SortIterType sortIterType, Bool isStable>
struct AddSortUTs
{
};

template <class Sort, SortType sortType, Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::RANDOM_ITER, isStable>
    : public UnitTest
{
    Void operator()()
    {
        UTRunner utRunner;
        utRunner.add<AddSortTCs<Sort, std::vector, sortType, isStable>>("Vector");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType, Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::BIDIR_ITER, isStable>
    : public UnitTest
{
    Void operator()()
    {
        UTRunner utRunner;
        utRunner.add<AddSortUTs<Sort, sortType, SortIterType::RANDOM_ITER, isStable>>("Random");
        utRunner.add<AddSortTCs<Sort, std::list, sortType, isStable>>("List");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType, Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::FORWARD_ITER, isStable>
    : public UnitTest
{
    Void operator()()
    {
        UTRunner utRunner;
        utRunner.add<AddSortUTs<Sort, sortType, SortIterType::BIDIR_ITER, isStable>>("Bidir");
        utRunner.add<AddSortTCs<Sort, std::forward_list, sortType, isStable>>("ForwardList");
        AD_UT_ASSERT(utRunner.run());
    }
};

}