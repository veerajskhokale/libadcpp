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

#ifndef AD_TEST_UTILITY_H_
#define AD_TEST_UTILITY_H_

#include "ad/mp/algorithm.h"
#include "ad/mp/functional.h"
#include "ad/mp/vector.h"
#include "ad/types.h"
#include "ad/unit_test.h"
#include "ad/utility.h"

#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace ad::mp;

using _ints = Vector_<ad::Int8, ad::Int16, ad::Int32, ad::Int64>;
using _uints = Vector_<ad::Uint8, ad::Uint16, ad::Uint32, ad::Uint64>;
using _intPairs =
    transform_<_ints, _ints, Vector_, TpFunc_<std::pair>::func_>::Result_;
using _uintPairs =
    transform_<_uints, _uints, Vector_, TpFunc_<std::pair>::func_>::Result_;
using _strings = Vector_<std::string>;

template <class UTPack>
struct UTAdder;

template <class... UTs>
struct UTAdder<Vector_<UTs...>> {
    void operator()(ad::UTRunner& utRunner) {
        int dummy[] = {(utRunner.add<UTs>(), 0)...};
        (void)dummy;
    }
};

namespace std {

template <class Val1, class Val2>
std::ostream& operator<<(std::ostream& out, const std::pair<Val1, Val2>& p) {
    out << "[" << p.first << "," << p.second << "]";
    return out;
}

} // namespace std

template <class Val>
struct RandomGenerator {
    using ValueType = Val;
    using RandomType = Val;
    using CompareType = std::less<ValueType>;

    RandomGenerator(
        ad::Size size = 0,
        RandomType min = std::numeric_limits<RandomType>::min(),
        RandomType max = std::numeric_limits<RandomType>::max())
        : mMt(std::random_device()()), mUd(min, max) {}

    auto operator()() { return mUd(mMt); }

private:
    std::mt19937 mMt;
    std::uniform_int_distribution<RandomType> mUd;
};

template <>
struct RandomGenerator<std::string> {
    using ValueType = std::string;
    using RandomType = ad::Int64;
    using CompareType = std::less<ValueType>;

    RandomGenerator(
        ad::Size size = 0,
        RandomType min = std::numeric_limits<RandomType>::min(),
        RandomType max = std::numeric_limits<RandomType>::max())
        : mMt(std::random_device()()), mUd(min, max) {}

    auto operator()() { return std::to_string(mUd(mMt)); }

private:
    std::mt19937 mMt;
    std::uniform_int_distribution<RandomType> mUd;
};

template <class Val>
struct RandomGenerator<std::pair<Val, Val>> {
    using ValueType = std::pair<Val, Val>;
    using RandomType = Val;
    using CompareType = std::less<ValueType>;

    RandomGenerator(
        ad::Size size = 0,
        RandomType min = std::numeric_limits<RandomType>::min(),
        RandomType max = std::numeric_limits<RandomType>::max())
        : mMt(std::random_device()()), mUd(min, max) {}

    auto operator()() { return std::make_pair(mUd(mMt), mUd(mMt)); }

private:
    std::mt19937 mMt;
    std::uniform_int_distribution<RandomType> mUd;
};

template <class Val, class Reverse = False_, class Compare = std::less<Val>>
struct SortedGenerator {
    using ValueType = Val;
    using RandomType = typename RandomGenerator<Val>::RandomType;
    using CompareType = Compare;

    SortedGenerator(
        ad::Size size = 1,
        RandomType min = std::numeric_limits<RandomType>::min(),
        RandomType max = std::numeric_limits<RandomType>::max())
        : mVec(size), mAt(0) {
        std::generate(
            mVec.begin(), mVec.end(), RandomGenerator<Val>(size, min, max));
        if (Reverse::value_) {
            std::sort(mVec.begin(), mVec.end(), CompareType());
        } else {
            std::sort(
                mVec.begin(), mVec.end(), [](const auto& l, const auto& r) {
                    return CompareType()(r, l);
                });
        }
    }

    auto operator()() { return mVec[mAt++]; }

private:
    std::vector<ValueType> mVec;
    ad::Size mAt;
};

#endif