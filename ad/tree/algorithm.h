/*
 * Copyright 2018 Veeraj S Khokale All Rights Reserved
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

#ifndef AD_TREE_ALGO_H_
#define AD_TREE_ALGO_H_

#include <algorithm>
#include <vector>
#include <type_traits>
#include "ad/tree/iterator.h"
#include "ad/tree/utility.h"

namespace ad
{
namespace tree
{

template <class Visitor, class Getter>
using GetterReturnType = std::remove_reference_t<
    std::result_of_t<Getter(typename Visitor::Reference)>>;

template <class Visitor, class Merge, class Init>
Void computeUp(Visitor root, Merge merge, Init init)
{
    for (auto pit = postBegin(root); pit != postEnd(root); ++pit) {
        init(*pit);
        merge(*pit, childBegin(pit.visitor()), childEnd(pit.visitor()));
    }
}

template <class Visitor, class Update, class Init>
Void computeDown(Visitor root, Update update, Init init)
{
    init(*root);
    for (auto pit = preBegin(root); pit != preEnd(root); ++pit) {
        for (auto cit = childBegin(pit.visitor());
            cit != childEnd(pit.visitor()); ++cit) {
            init(*cit);
            update(*pit, *cit);
        }
    }
}

template <class Visitor, class CountGetter, class CountSetter>
Void count(Visitor root, CountGetter getCount, CountSetter setCount)
{
    using CountType = GetterReturnType<Visitor, CountGetter>;

    computeUp(
        root,
        [getCount, setCount](auto& parent, auto first, auto last) {
            for (auto child = first; child != last; ++child) {
                setCount(parent, getCount(parent) + getCount(*child));
            }
        },
        [setCount](auto& node) {
            setCount(node, CountType(Size(1)));
        }
    );
}

template <class Visitor>
Size count(Visitor root)
{
    return std::distance(preBegin(root), preEnd(root));
}

template <class Visitor, class HeightGetter, class HeightSetter>
Void height(Visitor root, HeightGetter getHeight, HeightSetter setHeight)
{
    using HeightType = GetterReturnType<Visitor, HeightGetter>;

    computeUp(
        root,
        [getHeight, setHeight](auto& parent, auto first, auto last) {
            for (auto child = first; child != last; ++child) {
                setHeight(parent, std::max(
                    getHeight(parent), getHeight(*child) + HeightType(Size(1))
                ));
            }
        },
        [setHeight](auto& node) {
            setHeight(node, HeightType(Size(0)));
        }
    );
}

template <class Visitor, class IdGetter>
Size height(Visitor root, IdGetter getId)
{
    Map<Visitor, IdGetter, Size> heightMap(root, getId);
    height(root, heightMap.getter(), heightMap.setter());
    return heightMap[*root];
}

template <class Visitor, class DepthGetter, class DepthSetter>
Void depth(Visitor root, DepthGetter getDepth, DepthSetter setDepth)
{
    using DepthType = GetterReturnType<Visitor, DepthGetter>;

    computeDown(
        root,
        [getDepth, setDepth](const auto& parent, auto& child) {
            setDepth(child, getDepth(parent) + DepthType(Size(1)));
        },
        [setDepth](auto& node) {
            setDepth(node, DepthType(Size(0)));
        }
    );
}

template <class Visitor>
Visitor lca(Visitor u, Visitor v)
{
    std::vector<Visitor> upath(1, u);
    std::vector<Visitor> vpath(1, v);

    for (auto pit = parentBegin(u); pit != parentEnd(u); ++pit) {
        upath.push_back(pit.visitor());
    }
    for (auto pit = parentBegin(v); pit != parentEnd(v); ++pit) {
        vpath.push_back(pit.visitor());
    }

    auto uit = upath.rbegin();
    for (auto vit = vpath.rbegin(); uit != upath.rend() &&
         vit != vpath.rend() && *uit == *vit; ++uit, ++vit);

    return *(--uit);
}

template <class Visitor, class WeightGetter, class DistanceGetter, class DistanceSetter>
Void distance(Visitor root, WeightGetter getWeight,
    DistanceGetter getDistance, DistanceSetter setDistance,
    GetterReturnType<Visitor, DistanceGetter> rootDistance)
{
    computeDown(
        root,
        [getWeight, getDistance, setDistance](const auto& parent, auto& child) {
            setDistance(child, getWeight(child) + getDistance(parent));
        },
        [setDistance, rootDistance](auto& node) {
            setDistance(node, rootDistance);
        }
    );
}

template <class Visitor, class WeightGetter, class DistanceGetter, class DistanceSetter>
Void distanceAllPairs(Visitor source, WeightGetter getWeight,
    DistanceGetter getDistance, DistanceSetter setDistance,
    GetterReturnType<Visitor, DistanceGetter> sourceDistance)
{
    distance(source, getWeight, getDistance, setDistance, sourceDistance);

    auto fromChild = source;
    for (auto pit = parentBegin(source); pit != parentEnd(source); ++pit) {
        setDistance(*pit, getDistance(*fromChild) + getWeight(*fromChild));
        for (auto cit = childBegin(pit.visitor()); cit != childEnd(pit.visitor()); ++cit) {
            if (cit.visitor() != fromChild) {
                distance(cit.visitor(), getWeight, getDistance,
                    setDistance, getDistance(*pit) + getWeight(*cit));
            }
        }
        fromChild = pit.visitor();
    }
}

template <class Visitor, class WeightGetter>
auto distanceBetween(Visitor u, Visitor v, WeightGetter getWeight,
    GetterReturnType<Visitor, WeightGetter> udist)
{
    std::vector<Visitor> upath(1, u);
    std::vector<Visitor> vpath(1, v);

    auto child = u;
    for (auto pit = parentBegin(u); pit != parentEnd(u); ++pit) {
        upath.push_back(pit.visitor());
        udist = udist + getWeight(*child);
        child = pit.visitor();
    }
    child = v;
    for (auto pit = parentBegin(v); pit != parentEnd(v); ++pit) {
        vpath.push_back(pit.visitor());
        udist = udist + getWeight(*child);
        child = pit.visitor();
    }

    auto uit = upath.rbegin() + 1;
    for (auto vit = vpath.rbegin() + 1; uit != upath.rend() &&
         vit != vpath.rend() && *uit == *vit; ++uit, ++vit) {
        udist = udist - getWeight(**uit);
    };

    return udist;
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ALGO_H_ */