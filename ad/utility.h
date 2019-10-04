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

#ifndef AD_CORE_UTILITY_H_
#define AD_CORE_UTILITY_H_

#include <utility>
#include <type_traits>
#include <string>
#include <cxxabi.h>
#include <memory>

#include "ad/types.h"

namespace ad
{

template <class T>
struct Name
{
    std::string operator()()
    {
        return gccDemangle(typeid(T).name());
    }

private:
    std::string gccDemangle(const char* mangledName)
    {
        int status;
        char *demangledName = abi::__cxa_demangle(mangledName, NULL, NULL, &status);
        if (!status) {
            std::string ret(demangledName);
            std::free(demangledName);
            return ret;
        } else {
            return ":__UNKNOWN__:";
        }
    }

}; /* struct Name */

template <class T>
struct TmpBuff
{
    using ValueType     = T;
    using Pointer       = T*;
    using Reference     = T&;
    using Iterator      = T*;

    TmpBuff()
        : mMem(nullptr), mCapacity(0), mSize(0)
    {
    }

    TmpBuff(PtrDiff sz)
        : mMem(nullptr), mCapacity(0), mSize(0)
    {
        auto ret = std::get_temporary_buffer<T>(sz);
        mMem = ret.first;
        mCapacity = ret.second;
        mSize = mCapacity < sz ? mCapacity : sz;
    }

    TmpBuff(const TmpBuff&) = delete;
    TmpBuff(TmpBuff&&) = delete;
    TmpBuff& operator=(const TmpBuff&) = delete;
    TmpBuff& operator=(TmpBuff&&) = delete;

    ~TmpBuff()
    {
        std::return_temporary_buffer(mMem);
    }

    PtrDiff capacity() const
    {
        return mCapacity;
    }

    PtrDiff size() const
    {
        return mSize;
    }

    Iterator begin()
    {
        return mMem;
    }

    Iterator end()
    {
        return mMem + size();
    }

    Void init()
    {
        std::uninitialized_fill(begin(), end(), T());
    }

    Void init(const T& val)
    {
        std::uninitialized_fill(begin(), end(), val);
    }

    template <class InputIt>
    Void init(InputIt first, InputIt last)
    {
        std::uninitialized_copy(first, last, begin());
    }

private:
    T*          mMem;
    PtrDiff     mCapacity;
    PtrDiff     mSize;

}; /* struct TmpBuff */

namespace _utility
{

// Non constructibe and non destructible "dead" class,
// for use in disabling copy/move assigment operators
struct DeadClass
{
    DeadClass() = delete;
    DeadClass(const DeadClass&) = delete;
    DeadClass& operator=(const DeadClass&) = delete;
    ~DeadClass() = delete;
};

// References: libcxx pair, gcc pair, libcxx __compressed_pair implementations
template <class T, Size Idx, Bool applyEBCO =
    std::is_empty<T>::value && !std::is_final<T>::value>
class PairElem
{
    using PairElemType      = PairElem<T, Idx, applyEBCO>;

public:
    constexpr explicit PairElem()
        : value()
    {
    }

    template <class U, class = std::enable_if_t<
        !std::is_same<std::decay_t<U>, PairElemType>::value>>
    constexpr explicit PairElem(U&& other)
        : value(std::forward<U>(other))
    {
    }

    template <class... Args, Size... Indexes>
    explicit PairElem(std::piecewise_construct_t,
        std::tuple<Args...> args, std::index_sequence<Indexes...>)
        : value(std::forward<Args>(std::get<Indexes>(args))...)
    {
    }

    template <class U, class = std::enable_if_t<
        !std::is_same<std::decay_t<U>, PairElemType>::value>>
    PairElemType& operator=(U&& other)
    {
        value = std::forward<U>(other);
        return *this;
    }

    T& get()
    {
        return value;
    }

    const T& get() const
    {
        return value;
    }

private:
    T value;

}; /* class PairElem */

template <class T, Size Idx>
class PairElem<T, Idx, true>
    : private T
{
    using PairElemType      = PairElem<T, Idx, true>;
    using Value             = T;

public:
    constexpr explicit PairElem()
        : Value()
    {
    }

    template <class U, class = std::enable_if_t<
        !std::is_same<std::decay_t<U>, PairElemType>::value>>
    constexpr explicit PairElem(U&& other)
        : Value(std::forward<U>(other))
    {
    }

    template <class... Args, Size... Indexes>
    explicit PairElem(std::piecewise_construct_t,
        std::tuple<Args...> args, std::index_sequence<Indexes...>)
        : Value(std::forward<Args>(std::get<Indexes>(args))...)
    {
    }

    template <class U, class = std::enable_if_t<
        !std::is_same<std::decay_t<U>, PairElemType>::value>>
    PairElemType& operator=(U&& other)
    {
        Value::operator=(std::forward<U>(other));
        return *this;
    }

    T& get()
    {
        return *this;
    }

    const T& get() const
    {
        return *this;
    }

}; /* class PairElem<T, Idx, true> */

} /* namespace anonymous */

template <class T1, class T2>
class Pair
    : private _utility::PairElem<T1, 0>,
      private _utility::PairElem<T2, 1>
{
    using PairType          = Pair<T1, T2>;
    using FirstBase         = _utility::PairElem<T1, 0>;
    using SecondBase        = _utility::PairElem<T2, 1>;

public:
    using FirstType         = T1;
    using SecondType        = T2;

private:
    template <Bool Dummy = true>
    struct CheckArgs
    {
        template <class U1, class U2>
        static constexpr Bool enableDefault()
        {
            return std::is_default_constructible<U1>::value &&
                std::is_default_constructible<U2>::value;
        }

        template <class U1, class U2>
        static constexpr Bool enableExplicit()
        {
            return std::is_constructible<FirstType, U1>::value &&
                std::is_constructible<SecondType, U2>::value &&
                (!std::is_convertible<U1, FirstType>::value ||
                    !std::is_convertible<U2, SecondType>::value);
        }

        template <class U1, class U2>
        static constexpr Bool enableImplicit()
        {
            return std::is_constructible<FirstType, U1>::value &&
                std::is_constructible<SecondType, U2>::value &&
                std::is_convertible<U1, FirstType>::value &&
                std::is_convertible<U2, SecondType>::value;
        }
    };

public:
    // Todo: when and what is the use of making the
    // default constructor explicit?
    template <Bool Dummy = true, std::enable_if_t<
        CheckArgs<Dummy>::template enableDefault<FirstType,
            SecondType>(), Bool> = false>
    constexpr Pair()
        : FirstBase(), SecondBase()
    {
    }

    template <Bool Dummy = true, std::enable_if_t<
        CheckArgs<Dummy>::template enableExplicit<const FirstType&,
            const SecondType&>(), Bool> = false>
    constexpr explicit Pair(const FirstType& first_, const SecondType& second_)
        : FirstBase(first_), SecondBase(second_)
    {
    }

    template <Bool Dummy = true, std::enable_if_t<
        CheckArgs<Dummy>::template enableImplicit<const FirstType&,
            const SecondType&>(), Bool> = false>
    constexpr Pair(const FirstType& first_, const SecondType& second_)
        : FirstBase(first_), SecondBase(second_)
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableExplicit<U1&&, U2&&>(), Bool> = false>
    constexpr explicit Pair(U1&& first_, U2&& second_)
        : FirstBase(std::forward<U1>(first_)),
          SecondBase(std::forward<U2>(second_))
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableImplicit<U1&&, U2&&>(), Bool> = false>
    constexpr Pair(U1&& first_, U2&& second_)
        : FirstBase(std::forward<U1>(first_)),
          SecondBase(std::forward<U2>(second_))
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableExplicit<
            const U1&, const U2&>(), Bool> = false>
    constexpr explicit Pair(const Pair<U1, U2>& other)
        : FirstBase(other.first()), SecondBase(other.second())
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableImplicit<
            const U1&, const U2&>(), Bool> = false>
    constexpr Pair(const Pair<U1, U2>& other)
        : FirstBase(other.first()), SecondBase(other.second())
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableExplicit<U1&&, U2&&>(), Bool> = false>
    constexpr explicit Pair(Pair<U1, U2>&& other)
        : FirstBase(std::forward<U1>(other.first())),
          SecondBase(std::forward<U2>(other.second()))
    {
    }

    template <class U1, class U2, std::enable_if_t<
        CheckArgs<>::template enableImplicit<U1&&, U2&&>(), Bool> = false>
    constexpr Pair(Pair<U1, U2>&& other)
        : FirstBase(std::forward<U1>(other.first())),
          SecondBase(std::forward<U2>(other.second()))
    {
    }

    template <class... Args1, class... Args2>
    Pair(std::piecewise_construct_t pc,
         std::tuple<Args1...> firstArgs,
         std::tuple<Args2...> secondArgs)
        : FirstBase(pc, std::move(firstArgs),
            std::index_sequence_for<Args1...>()),
          SecondBase(pc, std::move(secondArgs),
            std::index_sequence_for<Args2...>())
    {
    }

    // Todo: when should copy and move constructors be constexpr?
    constexpr Pair(const PairType&) = default;
    constexpr Pair(PairType&&) = default;

    PairType& operator=(std::conditional_t<
        std::is_copy_assignable<FirstType>::value &&
        std::is_copy_assignable<SecondType>::value,
        const PairType&, const _utility::DeadClass&> other)
    {
        FirstBase::operator=(other.first());
        SecondBase::operator=(other.second());
        return *this;
    }

    template <class U1, class U2>
    std::enable_if_t<std::is_assignable<FirstType&, const U1&>::value &&
        std::is_assignable<SecondType&, const U2&>::value, PairType&>
        operator=(const Pair<U1, U2>& other)
    {
        FirstBase::operator=(other.first());
        SecondBase::operator=(other.second());
        return *this;
    }

    PairType& operator=(std::conditional_t<
        std::is_move_assignable<FirstType>::value &&
        std::is_move_assignable<SecondType>::value,
        PairType&&, _utility::DeadClass&&> other)
    {
        FirstBase::operator=(std::move(other.first()));
        SecondBase::operator=(std::move(other.second()));
        return *this;
    }

    template <class U1, class U2>
    std::enable_if_t<std::is_assignable<FirstType&, U1&&>::value &&
        std::is_assignable<SecondType&, U2&&>::value, PairType&>
        operator=(Pair<U1, U2>&& other)
    {
        FirstBase::operator=(std::forward<U1>(other.first()));
        SecondBase::operator=(std::forward<U2>(other.second()));
        return *this;
    }

    FirstType& first() &
    {
        return FirstBase::get();
    }

    const FirstType& first() const&
    {
        return FirstBase::get();
    }

    FirstType&& first() &&
    {
        return std::move(FirstBase::get());
    }

    const FirstType&& first() const&&
    {
        return std::move(FirstBase::get());
    }

    SecondType& second() &
    {
        return SecondBase::get();
    }

    const SecondType& second() const&
    {
        return SecondBase::get();
    }

    SecondType&& second() &&
    {
        return std::move(SecondBase::get());
    }

    const SecondType&& second() const&&
    {
        return std::move(SecondBase::get());
    }

}; /* class Pair */

} /* namespace ad */

#endif /* AD_CORE_UTILITY_H_ */