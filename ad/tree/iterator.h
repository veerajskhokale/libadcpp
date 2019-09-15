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

#ifndef AD_TREE_ITER_H_
#define AD_TREE_ITER_H_

#include <iterator>
#include "ad/types.h"
#include "ad/mp/utility.h"
#include "ad/tree/visitor.h"

namespace ad
{
namespace tree
{

template <class ForwardVs>
ForwardVs leftLowestDescendant(ForwardVs v)
{
    for (; v.first(); v = v.first());
    return v;
}

template <class BidirVs>
BidirVs rightLowestDescendant(BidirVs v)
{
    for (; v.last(); v = v.last());
    return v;
}

template <class ForwardVs>
ForwardVs preNext(ForwardVs v)
{
    if (v.first()) {
        return v.first();
    } else {
        for (; v && !v.right(); v = v.parent());
        return v ? v.right() : v;
    }
}

template <class BidirVs>
BidirVs prePrev(BidirVs v)
{
    if (v.left()) {
        return rightLowestDescendant(v.left());
    } else {
        return v.parent();
    }
}

template <class ForwardVs>
ForwardVs postNext(ForwardVs v)
{
    if (v.right()) {
        return leftLowestDescendant(v.right());
    } else {
        return v.parent();
    }
}

template <class BidirVs>
BidirVs postPrev(BidirVs v)
{
    if (v.last()) {
        return v.last();
    } else {
        for (; v && !v.left(); v = v.parent());
        return v ? v.left() : v;
    }
}

template <class ForwardVs>
class PreIterator
{
    using IterType              = PreIterator<ForwardVs>;
    using VsTraits              = VisitorTraits<ForwardVs>;

public:
    using VisitorType           = typename VsTraits::VisitorType;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VsTraits::ValueType;
    using DifferenceType        = typename VsTraits::DifferenceType;
    using Pointer               = typename VsTraits::Pointer;
    using Reference             = typename VsTraits::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PreIterator()
        : mCur()
    {
    }

    template <class ForwardVs_>
    PreIterator(const PreIterator<ForwardVs_>& other)
        : mCur(other.visitor())
    {
    }

    template <class ForwardVs_>
    IterType& operator=(const PreIterator<ForwardVs_>& other)
    {
        mCur = other.visitor();
        return *this;
    }

    VisitorType visitor() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return mCur.operator->();
    }

    IterType& operator++()
    {
        mCur = preNext(mCur);
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType v)
    {
        return IterType(v);
    }

    inline static IterType end(VisitorType v)
    {
        if (!v) {
            return IterType(v);
        } else {
            for (; v && !v.right(); v = v.parent());
            return v ? IterType(v.right()) : IterType(v);
        }
    }

private:
    explicit PreIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class PreIterator */

template <class ForwardVs1, class ForwardVs2>
inline Bool operator==(const PreIterator<ForwardVs1>& l,
    const PreIterator<ForwardVs2>& r)
{
    return l.visitor() == r.visitor();
}

template <class ForwardVs1, class ForwardVs2>
inline Bool operator!=(const PreIterator<ForwardVs1>& l,
    const PreIterator<ForwardVs2>& r)
{
    return !(l == r);
}

template <class ForwardVs>
class PostIterator
{
    using IterType              = PostIterator<ForwardVs>;
    using VsTraits              = VisitorTraits<ForwardVs>;

public:
    using VisitorType           = typename VsTraits::VisitorType;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VsTraits::ValueType;
    using DifferenceType        = typename VsTraits::DifferenceType;
    using Pointer               = typename VsTraits::Pointer;
    using Reference             = typename VsTraits::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PostIterator()
        : mCur()
    {
    }

    template <class ForwardVs_>
    PostIterator(const PostIterator<ForwardVs_>& other)
        : mCur(other.visitor())
    {
    }

    template <class ForwardVs_>
    IterType& operator=(const PostIterator<ForwardVs_>& other)
    {
        mCur = other.visitor();
        return *this;
    }

    VisitorType visitor() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return mCur.operator->();
    }

    IterType& operator++()
    {
        mCur = postNext(mCur);
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType v)
    {
        return v ? IterType(leftLowestDescendant(v)) : IterType(v);
    }

    inline static IterType end(VisitorType v)
    {
        return v ? IterType(postNext(v)) : IterType(v);
    }

private:
    explicit PostIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class PostIterator */

template <class ForwardVs1, class ForwardVs2>
inline Bool operator==(const PostIterator<ForwardVs1>& l,
    const PostIterator<ForwardVs2>& r)
{
    return l.visitor() == r.visitor();
}

template <class ForwardVs1, class ForwardVs2>
inline Bool operator!=(const PostIterator<ForwardVs1>& l,
    const PostIterator<ForwardVs2>& r)
{
    return !(l == r);
}

template <class ForwardVs>
class ChildIterator
{
    using IterType              = ChildIterator<ForwardVs>;
    using VsTraits              = VisitorTraits<ForwardVs>;

public:
    using VisitorType           = typename VsTraits::VisitorType;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VsTraits::ValueType;
    using DifferenceType        = typename VsTraits::DifferenceType;
    using Pointer               = typename VsTraits::Pointer;
    using Reference             = typename VsTraits::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    ChildIterator()
        : mCur()
    {
    }

    template <class ForwardVs_>
    ChildIterator(const ChildIterator<ForwardVs_>& other)
        : mCur(other.visitor())
    {
    }

    template <class ForwardVs_>
    IterType& operator=(const ChildIterator<ForwardVs_>& other)
    {
        mCur = other.visitor();
        return *this;
    }

    VisitorType visitor() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return mCur.operator->();
    }

    IterType& operator++()
    {
        mCur = mCur.right();
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType v)
    {
        return v ? IterType(v.first()) : IterType(v);
    }

    inline static IterType end(VisitorType v)
    {
        return IterType();
    }

private:
    explicit ChildIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class ChildIterator */

template <class ForwardVs1, class ForwardVs2>
inline Bool operator==(const ChildIterator<ForwardVs1>& l,
    const ChildIterator<ForwardVs2>& r)
{
    return l.visitor() == r.visitor();
}

template <class ForwardVs1, class ForwardVs2>
inline Bool operator!=(const ChildIterator<ForwardVs1>& l,
    const ChildIterator<ForwardVs2>& r)
{
    return !(l == r);
}

template <class ParentVs>
class ParentIterator
{
    using IterType              = ParentIterator<ParentVs>;
    using VsTraits              = VisitorTraits<ParentVs>;

public:
    using VisitorType           = typename VsTraits::VisitorType;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VsTraits::ValueType;
    using DifferenceType        = typename VsTraits::DifferenceType;
    using Pointer               = typename VsTraits::Pointer;
    using Reference             = typename VsTraits::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    ParentIterator()
        : mCur()
    {
    }

    template <class ParentVs_>
    ParentIterator(const ParentIterator<ParentVs_>& other)
        : mCur(other.visitor())
    {
    }

    template <class ParentVs_>
    IterType& operator=(const ParentIterator<ParentVs_>& other)
    {
        mCur = other.visitor();
        return *this;
    }

    VisitorType visitor() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return mCur.operator->();
    }

    IterType& operator++()
    {
        mCur = mCur.parent();
        return *this;
    }

    IterType& operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType v)
    {
        return v ? IterType(v.parent()) : IterType(v);
    }

    inline static IterType end(VisitorType v)
    {
        return IterType();
    }

private:
    explicit ParentIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType     mCur;

}; /* class ParentIterator */

template <class ParentVs1, class ParentVs2>
inline Bool operator==(const ParentIterator<ParentVs1>& l,
    const ParentIterator<ParentVs2>& r)
{
    return l.visitor() == r.visitor();
}

template <class ParentVs1, class ParentVs2>
inline Bool operator!=(const ParentIterator<ParentVs1>& l,
    const ParentIterator<ParentVs2>& r)
{
    return !(l == r);
}

template <class ForwardVs>
inline auto preBegin(ForwardVs v)
{
    return PreIterator<ForwardVs>::begin(v);
}

template <class ForwardVs>
inline auto preEnd(ForwardVs v)
{
    return PreIterator<ForwardVs>::end(v);
}

template <class ForwardVs>
inline auto postBegin(ForwardVs v)
{
    return PostIterator<ForwardVs>::begin(v);
}

template <class ForwardVs>
inline auto postEnd(ForwardVs v)
{
    return PostIterator<ForwardVs>::end(v);
}

template <class ForwardVs>
inline auto childBegin(ForwardVs v)
{
    return ChildIterator<ForwardVs>::begin(v);
}

template <class ForwardVs>
inline auto childEnd(ForwardVs v)
{
    return ChildIterator<ForwardVs>::end(v);
}

template <class ParentVs>
inline auto parentBegin(ParentVs v)
{
    return ParentIterator<ParentVs>::begin(v);
}

template <class ParentVs>
inline auto parentEnd(ParentVs v)
{
    return ParentIterator<ParentVs>::end(v);
}

template <class ForwardVs>
inline auto preIters(ForwardVs v)
{
    return std::make_pair(preBegin(v), preEnd(v));
}

template <class ForwardVs>
inline auto postIters(ForwardVs v)
{
    return std::make_pair(postBegin(v), postEnd(v));
}

template <class ForwardVs>
inline auto childIters(ForwardVs v)
{
    return std::make_pair(childBegin(v), childEnd(v));
}

template <class ParentVs>
inline auto parentIters(ParentVs v)
{
    return std::make_pair(parentBegin(v), parentEnd(v));
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ITER_H_ */