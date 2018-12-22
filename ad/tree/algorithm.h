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

namespace ad
{
namespace tree
{

template <class Visitor, class Func>
Void computeUp(Visitor root, Func func)
{
    for (auto pit = postBegin(root); pit != postEnd(root); ++pit) {
        func(pit.visitor(), childBegin(pit.visitor()), childEnd(pit.visitor()));
    }
}

template <class Visitor, class Func>
Void computeDown(Visitor root, Func func)
{
    for (auto pit = preBegin(root); pit != preEnd(root); ++pit) {
        func(pit.visitor(), childBegin(pit.visitor()), childEnd(pit.visitor()));
    }
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ALGO_H_ */