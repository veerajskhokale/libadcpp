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

#include <iostream>
#include <algorithm>
#include "ad/tree/iterator.h"
#include "ad/tree/utility.h"

namespace ad
{
namespace tree
{

template <class Visitor, class Update, class Init>
void computeUp(Visitor root, Update upd, Init init)
{
    for (auto pit = postBegin(root); pit != postEnd(root); ++pit) {
        init(*pit);
        for (auto cit = childBegin(pit.visitor());
            cit != childEnd(pit.visitor()); ++cit) {
            upd(*pit, *cit);
        }
    }
}

template <class Visitor, class Update, class Init>
void computeDown(Visitor root, Update upd, Init init)
{
    init(*root);
    for (auto pit = preBegin(root); pit != preEnd(root); ++pit) {
        for (auto cit = childBegin(pit.visitor());
            cit != childEnd(pit.visitor()); ++cit) {
            init(*cit);
            upd(*pit, *cit);
        }
    }
}

template <class Visitor, class CountGetter, class CountSetter>
void count(Visitor root, CountGetter getCount, CountSetter setCount)
{
    computeUp(root,
        [getCount, setCount](auto& parent, const auto& child) {
            setCount(parent, getCount(parent) + getCount(child));
        },
        [setCount](auto& node) {
            setCount(node, 1);
        });
}

template <class Visitor>
Size count(Visitor root)
{
    return std::distance(preBegin(root), preEnd(root));
}

template <class Visitor, class HeightGetter, class HeightSetter>
void height(Visitor root, HeightGetter getHeight, HeightSetter setHeight)
{
    computeUp(root,
        [getHeight, setHeight](auto& parent, const auto& child) {
            setHeight(parent, std::max(getHeight(parent), getHeight(child) + 1));
        },
        [setHeight](auto& node) {
            setHeight(node, 0);
        });
}

template <class Visitor, class IdGetter>
Size height(Visitor root, IdGetter getId)
{
    Map<Visitor, IdGetter, Size> heightMap(root, getId);
    height(root, heightMap.getter(), heightMap.setter());
    return heightMap[*root];
}

template <class Visitor, class DepthGetter, class DepthSetter>
void depth(Visitor root, DepthGetter getDepth, DepthSetter setDepth)
{
    computeDown(root,
        [getDepth, setDepth](const auto& parent, auto& child) {
            setDepth(child, getDepth(parent) + 1);
        },
        [setDepth](auto& node) {
            setDepth(node, 0);
        });
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ALGO_H_ */