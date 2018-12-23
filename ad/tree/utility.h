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

#ifndef AD_TREE_UTILITY_H_
#define AD_TREE_UTILITY_H_

#include "ad/types.h"
#include "ad/tree/iterator.h"
#include <vector>
#include <utility>

namespace ad
{
namespace tree
{

template <class Visitor, class IdGetter, class Val>
class Map
{
    using MapType               = Map<Visitor, IdGetter, Val>;

public:
    using VisitorType           = Visitor;
    using KeyType               = typename VisitorType::ValueType;
    using IdGetterType          = IdGetter;
    using ValueType             = Val;
    using Reference             = ValueType&;
    using ConstReference        = const ValueType&;

    Map(VisitorType root, IdGetterType getId)
        : mGetId(getId), mMinId(), mVec()
    {
        resizeVector(root);
    }

    Map(const MapType&) = default;

    void reset(VisitorType root, IdGetterType getId)
    {
        mGetId = getId;
        resizeVector(root);
    }

    void clear()
    {
        mVec.clear();
    }

    auto getter() const
    {
        return [this](const KeyType& key) {
            return mVec[this->mGetId(key) - mMinId];
        };
    }

    auto setter()
    {
        return [this](const KeyType& key, const Val& value) {
            this->mVec[this->mGetId(key) - mMinId] = value;
        };
    }

    Reference operator[](const KeyType& key)
    {
        return mVec[mGetId(key) - mMinId];
    }

    ConstReference operator[](const KeyType& key) const
    {
        return mVec[mGetId(key) - mMinId];
    }

private:
    void resizeVector(VisitorType root)
    {
        mMinId = mGetId(*root);
        Size maxId = mGetId(*root);
        for (auto it = preBegin(root); it != preEnd(root); ++it) {
            mMinId = std::min(mMinId, mGetId(*it));
            maxId = std::max(maxId, mGetId(*it));
        }
        mVec.resize(maxId - mMinId + 1);
    }

    IdGetterType                mGetId;
    Size                        mMinId;
    std::vector<ValueType>      mVec;

}; /* class Map */

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_UTILITY_H_ */