#ifndef AD_TEST_UTIL_H_
#define AD_TEST_UTIL_H_

#include <string>
#include <utility>
#include <vector>
#include "ad/types.h"
#include "ad/util.h"

template <class Object, class RandomType>
struct FromRandom
{
    auto operator()(RandomType randomValue)
    {
        return ad::makeObject<Object>(randomValue);
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

template <class RandomType>
struct FromRandom<std::pair<RandomType, RandomType>, RandomType>
{
    auto operator()(RandomType randomValue)
    {
        return std::make_pair(randomValue, randomValue);
    }
};

template <class RandomType>
struct FromRandom<std::vector<RandomType>, RandomType>
{
    auto operator()(RandomType randomValue)
    {
        ad::Size size = 100;
        std::vector<RandomType> vec(size);
        for (ad::Size i = 0; i < size; ++i) {
            vec[i] = randomValue;
        }
        return vec;
    }
};

template <class Object, class RandomType>
inline auto fromRandom(RandomType randomValue)
{
    return FromRandom<Object, RandomType>()(randomValue);
}

#endif