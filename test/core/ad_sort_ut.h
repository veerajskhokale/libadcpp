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
#include "ad_test_util.h"

template <class InputIt1, class InputIt2,
    class InputIt3, class Stream>
ad::Void printDiagnostics(InputIt1 inFirst, InputIt1 inLast,
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
ad::Void verify(InputIt inFirst, InputIt inLast,
    Compare comp, ad::Bool isStable, Sort sort, Args&&... args)
{
    using ValueType = typename
        std::iterator_traits<InputIt>::value_type;

    auto cont = ad::makeObject<Container>(inFirst, inLast);
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
    ad::Void operator()(Compare comp, ad::Bool isStable, Sort sort, Args&&... args)
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
    ad::Void operator()(Compare comp, ad::Bool isStable, Sort sort, Args&&... args)
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
    ad::Void operator()(Compare comp, ad::Bool isStable, Sort sort, Args&&... args)
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
    class RandomType, template <typename> class Compare, ad::Bool isStable>
struct CompSortWithComp
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            Compare<ValType>(), isStable, CompSort(), Compare<ValType>()
        );
    }
};

template <template <typename, typename> class TC, class CompSort,
    template <typename, typename> class Container,
    class ValType, class RandomType, ad::Bool isStable>
struct CompSortWithoutComp
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            std::less<ValType>(), isStable, CompSort()
        );
    }
};

template <template <typename, typename> class TC, class IntSort,
    template <typename, typename> class Container,
    class ValType, class RandomType, ad::Bool isStable>
struct IntegerSort
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        TC<Container<ValType, std::allocator<ValType>>, RandomType>()(
            std::less<ValType>(), isStable, IntSort()
        );
    }
};

template <template <typename, typename> class TC, class Sort,
    template <typename, typename> class Container,
    SortType sortType, ad::Bool isStable>
struct AddSortTypes
{
};

template <template <typename, typename> class TC, class IntSort,
    template <typename, typename> class Container, ad::Bool isStable>
struct AddSortTypes<TC, IntSort, Container, SortType::INT_SORT, isStable>
    : public ad::UnitTest
{
    template <class IntType>
    ad::Void addHelper(ad::UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, IntSort, Container, IntType, IntType, isStable>>(name);
    }

    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        addHelper<ad::Int8>(utRunner, "Int8");
        addHelper<ad::Int16>(utRunner, "Int16");
        addHelper<ad::Int32>(utRunner, "Int32");
        addHelper<ad::Int64>(utRunner, "Int64");
        addHelper<ad::Uint8>(utRunner, "Uint8");
        addHelper<ad::Uint16>(utRunner, "Uint16");
        addHelper<ad::Uint32>(utRunner, "Uint32");
        addHelper<ad::Uint64>(utRunner, "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC,
    template <typename, typename> class Container, ad::Bool isStable>
struct AddSortTypes<TC, ad::CountingSort, Container, SortType::INT_SORT, isStable>
    : public ad::UnitTest
{
    template <class IntType, class RandomType>
    ad::Void addHelper(ad::UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, ad::CountingSort, Container, IntType, RandomType, isStable>>(name);
    }

    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        addHelper<ad::Int16, ad::Int8>(utRunner, "Int16-8");
        addHelper<ad::Int32, ad::Int16>(utRunner, "Int32-16");
        addHelper<ad::Int64, ad::Int16>(utRunner, "Int64-16");
        addHelper<ad::Uint8, ad::Uint8>(utRunner, "Uint8");
        addHelper<ad::Uint16, ad::Uint16>(utRunner, "Uint16");
        addHelper<ad::Uint32, ad::Uint16>(utRunner, "Uint32-16");
        addHelper<ad::Uint64, ad::Uint16>(utRunner, "Uint64-16");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC,
    template <typename, typename> class Container, ad::Bool isStable>
struct AddSortTypes<TC, ad::RadixSort, Container, SortType::INT_SORT, isStable>
    : public ad::UnitTest
{
    template <class IntType, class RandomType>
    ad::Void addHelper(ad::UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<IntegerSort<TC, ad::RadixSort, Container, IntType, RandomType, isStable>>(name);
    }

    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        addHelper<ad::Int16, ad::Int8>(utRunner, "Int16-8");
        addHelper<ad::Int32, ad::Int16>(utRunner, "Int32-16");
        addHelper<ad::Int64, ad::Int16>(utRunner, "Int64-16");
        addHelper<ad::Uint8, ad::Uint8>(utRunner, "Uint8");
        addHelper<ad::Uint16, ad::Uint16>(utRunner, "Uint16");
        addHelper<ad::Uint32, ad::Uint32>(utRunner, "Uint32");
        addHelper<ad::Uint64, ad::Uint64>(utRunner, "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <typename, typename> class TC, class CompSort,
    template <typename, typename> class Container, ad::Bool isStable>
struct AddSortTypes<TC, CompSort, Container, SortType::COMP_SORT, isStable>
    : public ad::UnitTest
{
    template <class IntType>
    ad::Void addHelper(ad::UTRunner& utRunner, const std::string& name)
    {
        utRunner.add<CompSortWithComp<TC, CompSort, Container, IntType, IntType, std::less, isStable>>(name + "Less");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, IntType, IntType, std::greater, isStable>>(name + "Greater");
    }

    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        const std::string COMP_SORT_NAME = "CompSort";
        utRunner.add<AddSortTypes<TC, CompSort, Container, SortType::INT_SORT, isStable>>("CompSort");
        utRunner.add<CompSortWithoutComp<TC, CompSort, Container, std::string, ad::Int64, isStable>>(COMP_SORT_NAME + "StringI64");
        utRunner.add<CompSortWithoutComp<TC, CompSort, Container, std::string, ad::Uint64, isStable>>(COMP_SORT_NAME + "StringU64");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, std::string, ad::Int64, std::less, isStable>>(COMP_SORT_NAME + "StringLess");
        utRunner.add<CompSortWithComp<TC, CompSort, Container, std::string, ad::Uint64, std::greater, isStable>>(COMP_SORT_NAME + "StringGreater");
        addHelper<ad::Int8>(utRunner, COMP_SORT_NAME + "Int8");
        addHelper<ad::Int16>(utRunner, COMP_SORT_NAME + "Int16");
        addHelper<ad::Int32>(utRunner, COMP_SORT_NAME + "Int32");
        addHelper<ad::Int64>(utRunner, COMP_SORT_NAME + "Int64");
        addHelper<ad::Uint8>(utRunner, COMP_SORT_NAME + "Uint8");
        addHelper<ad::Uint16>(utRunner, COMP_SORT_NAME + "Uint16");
        addHelper<ad::Uint32>(utRunner, COMP_SORT_NAME + "Uint32");
        addHelper<ad::Uint64>(utRunner, COMP_SORT_NAME + "Uint64");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, template <typename, typename> class Container,
    SortType sortType, ad::Bool isStable>
struct AddSortTCs
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<AddSortTypes<BorderTC, Sort, Container, sortType, isStable>>("BorderTC");
        utRunner.add<AddSortTypes<RandomTC, Sort, Container, sortType, isStable>>("RandomTC");
        utRunner.add<AddSortTypes<SortedTC, Sort, Container, sortType, isStable>>("SortedTC");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType,
    SortIterType sortIterType, ad::Bool isStable>
struct AddSortUTs
{
};

template <class Sort, SortType sortType, ad::Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::RANDOM_ITER, isStable>
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<AddSortTCs<Sort, std::vector, sortType, isStable>>("Vector");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType, ad::Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::BIDIR_ITER, isStable>
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<AddSortUTs<Sort, sortType, SortIterType::RANDOM_ITER, isStable>>("Random");
        utRunner.add<AddSortTCs<Sort, std::list, sortType, isStable>>("List");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, SortType sortType, ad::Bool isStable>
struct AddSortUTs<Sort, sortType, SortIterType::FORWARD_ITER, isStable>
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<AddSortUTs<Sort, sortType, SortIterType::BIDIR_ITER, isStable>>("Bidir");
        utRunner.add<AddSortTCs<Sort, std::forward_list, sortType, isStable>>("ForwardList");
        AD_UT_ASSERT(utRunner.run());
    }
};