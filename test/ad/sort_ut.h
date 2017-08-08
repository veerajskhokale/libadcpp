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

#include "ad/sort.h"
#include "ad/ut.h"
#include "ad/util.h"
#include "test/util.h"
#include "ad/mp/util.h"
#include "ad/mp/algorithm.h"

using namespace ad::mp;

struct IntegerSort;
struct ComparisionSort;
struct RandomAccess;
struct BidirAccess;
struct ForwardAccess;

template <class InputIt1, class InputIt2,
    class InputIt3, class Stream>
void printDiagnostics(InputIt1 inFirst, InputIt1 inLast,
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

template <class Sort, class WithCompare, class Compare, class IsStable>
struct CallSort;

template <class Sort, class Compare, class IsStable>
struct CallSort<Sort, True_, Compare, IsStable>
{
    template <class InputIt1, class InputIt2>
    void operator()(InputIt1 contFirst, InputIt1 contLast,
        InputIt2 expectedFirst, InputIt2 expectedLast)
    {
        Sort()(contFirst, contLast, Compare());
        if (IsStable::Value_) {
            std::stable_sort(expectedFirst, expectedLast, Compare());
        } else {
            std::sort(expectedFirst, expectedLast, Compare());
        }
    }
};

template <class Sort, class Compare, class IsStable>
struct CallSort<Sort, False_, Compare, IsStable>
{
    template <class InputIt1, class InputIt2>
    void operator()(InputIt1 contFirst, InputIt1 contLast,
        InputIt2 expectedFirst, InputIt2 expectedLast)
    {
        Sort()(contFirst, contLast);
        if (IsStable::Value_) {
            std::stable_sort(expectedFirst, expectedLast);
        } else {
            std::sort(expectedFirst, expectedLast);
        }
    }
};

template <class Sort, class Generator>
struct Generate
{
    template <class InputIt>
    void operator()(InputIt first, InputIt last, ad::Size size)
    {
        using RandomType = typename Generator::RandomType;
        std::generate(first, last, Generator(size));
    }
};

template <class Generator>
struct Generate<ad::CountingSort, Generator>
{
    template <class InputIt>
    void operator()(InputIt first, InputIt last, ad::Size size)
    {
        using RandomType = typename Generator::RandomType;
        RandomType min = std::numeric_limits<RandomType>::min() / 2 + 1;
        RandomType max = std::numeric_limits<RandomType>::max() / 2 - 1;
        max = (((double)max - (double)min) > 16000) ? (min + 16000) : max;
        std::generate(first, last, Generator(size, min, max));
    }
};

template <class Generator>
struct Generate<ad::RadixSort, Generator>
{
    template <class InputIt>
    void operator()(InputIt first, InputIt last, ad::Size size)
    {
        using RandomType = typename Generator::RandomType;
        RandomType min = std::numeric_limits<RandomType>::min() / 2 + 1;
        RandomType max = std::numeric_limits<RandomType>::max() / 2 - 1;
        std::generate(first, last, Generator(size, min, max));
    }
};

template <class Sort, class Container, class Generator,
    class WithCompare, class IsStable>
struct Verify
    : public ad::UnitTest
{
    void operator()()
    {
        using ValueType = typename Generator::ValueType;
        using ContainerType = typename apply_<Container, ValueType>::Result_;
        using CompareType = typename Generator::CompareType;

        for (ad::Size size = 0; size < 1000; ++size) {
            std::vector<ValueType> in(size);
            Generate<Sort, Generator>()(in.begin(), in.end(), size);
            std::vector<ValueType> expected(in.begin(), in.end());
            auto cont = ContainerType(in.begin(), in.end());

            CallSort<Sort, WithCompare, CompareType, IsStable>()
                (cont.begin(), cont.end(), expected.begin(), expected.end());

            auto isEqual = std::equal(cont.begin(),
                cont.end(), expected.begin());

            AD_UT_ASSERT(isEqual, [=](auto& strm) {
                printDiagnostics(in.begin(), in.end(), cont.begin(),
                    cont.end(), expected.begin(), expected.end(), strm);
            });
        }
    }
};

template <class Sort, class Container, class Type, class IsStable>
struct TypeSortReq;

template <class Sort, class Container, class IsStable>
struct TypeSortReq<Sort, Container, IntegerSort, IsStable>
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _allInts = PackOps_::concat_<_ints, _uints>::Result_;
        using _randomAllIntGenerators = forEach_<_allInts,
            RandomGenerator<_0>>::Result_;
        using _sortedAllIntGenerators = forEach_<_allInts,
            SortedGenerator<_0>>::Result_;
        using _reverseSortedAllIntGenerators = forEach_<_allInts,
            SortedGenerator<_0, True_>>::Result_;
        using _allIntGenerators = PackOps_::concat_<_randomAllIntGenerators,
            _sortedAllIntGenerators, _reverseSortedAllIntGenerators>::Result_;
        using _utPack = typename forEach_<_allIntGenerators,
            Verify<Sort, Container, _0, False_, IsStable>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, class Container, class IsStable>
struct TypeSortReq<Sort, Container, ComparisionSort, IsStable>
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<TypeSortReq<Sort, Container, IntegerSort, IsStable>>();
        using _objects = PackOps_::concat_<_strings,
            _intPairs, _uintPairs>::Result_;
        using _randomObjectGenerators = forEach_<_objects,
            RandomGenerator<_0>>::Result_;
        using _sortedObjectGenerators = forEach_<_objects,
            SortedGenerator<_0>>::Result_;
        using _reverseSortedObjectGenerators = forEach_<_objects,
            SortedGenerator<_0, True_>>::Result_;
        using _objectGenerators = PackOps_::concat_<_randomObjectGenerators,
            _sortedObjectGenerators, _reverseSortedObjectGenerators>::Result_;
        using _utPack1 = typename forEach_<_objectGenerators,
            Verify<Sort, Container, _0, True_, IsStable>>::Result_;
        using _utPack2 = typename forEach_<_objectGenerators,
            Verify<Sort, Container, _0, False_, IsStable>>::Result_;
        using _utPack = typename PackOps_::concat_<_utPack1, _utPack2>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, class Iter, class Type, class IsStable>
struct IterSortReq;

template <class Sort, class Type, class IsStable>
struct IterSortReq<Sort, RandomAccess, Type, IsStable>
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _randomAccessContainers = Pack_<Pred_<std::vector>>;
        using _utPack = typename forEach_<_randomAccessContainers,
            TypeSortReq<Sort, _0, Type, IsStable>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, class Type, class IsStable>
struct IterSortReq<Sort, BidirAccess, Type, IsStable>
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<IterSortReq<Sort, RandomAccess, Type, IsStable>>();
        using _bidirAccessContainers = Pack_<Pred_<std::list>>;
        using _utPack = typename forEach_<_bidirAccessContainers,
            TypeSortReq<Sort, _0, Type, IsStable>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, class Type, class IsStable>
struct IterSortReq<Sort, ForwardAccess, Type, IsStable>
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<IterSortReq<Sort, BidirAccess, Type, IsStable>>();
        using _forwardAccessContainers = Pack_<Pred_<std::forward_list>>;
        using _utPack = typename forEach_<_forwardAccessContainers,
            TypeSortReq<Sort, _0, Type, IsStable>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Sort, class Iter, class Type, class IsStable>
struct SortReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<IterSortReq<Sort, Iter, Type, IsStable>>();
        AD_UT_ASSERT(utRunner.run());
    }
};