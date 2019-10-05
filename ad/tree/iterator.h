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
ForwardVs rightLowestAncestor(ForwardVs v)
{
    for (; v && !v.right(); v = v.parent());
    return v ? v.right() : v;
}

template <class BidirVs>
BidirVs leftLowestAncestor(BidirVs v)
{
    for (; v && !v.left(); v = v.parent());
    return v ? v.left() : v;
}

template <class ForwardVs>
ForwardVs preNext(ForwardVs v)
{
    if (v.first()) {
        return v.first();
    } else {
        return rightLowestAncestor(v);
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
        return leftLowestAncestor(v);
    }
}

template <class ForwardVs>
ForwardVs leafNext(ForwardVs v)
{
    v = rightLowestAncestor(v);
    return v ? leftLowestDescendant(v) : v;
}

template <class BidirVs>
BidirVs leafPrev(BidirVs v)
{
    v = leftLowestAncestor(v);
    return v ? rightLowestDescendant(v) : v;
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

    inline static IterType begin(VisitorType root)
    {
        return IterType(root);
    }

    inline static IterType end(VisitorType root)
    {
        return IterType(rightLowestAncestor(root));
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

    inline static IterType begin(VisitorType root)
    {
        return IterType(leftLowestDescendant(root));
    }

    inline static IterType end(VisitorType root)
    {
        return IterType(postNext(root));
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

    inline static IterType begin(VisitorType parent)
    {
        return IterType(parent.first());
    }

    inline static IterType end(VisitorType parent)
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

    inline static IterType begin(VisitorType child)
    {
        return IterType(child.parent());
    }

    inline static IterType end(VisitorType child)
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
class LeafIterator
{
    using IterType              = LeafIterator<ForwardVs>;
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

    LeafIterator()
        : mCur()
    {
    }

    template <class ForwardVs_>
    LeafIterator(const LeafIterator<ForwardVs_>& other)
        : mCur(other.visitor())
    {
    }

    template <class ForwardVs_>
    IterType& operator=(const LeafIterator<ForwardVs_>& other)
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
        mCur = leafNext(mCur);
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType root)
    {
        return IterType(leftLowestDescendant(root));
    }

    inline static IterType end(VisitorType root)
    {
        return IterType(leafNext(root));
    }

private:
    explicit LeafIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType     mCur;

}; /* class LeafIterator */

template <class ForwardVs1, class ForwardVs2>
inline Bool operator==(const LeafIterator<ForwardVs1>& l,
    const LeafIterator<ForwardVs2>& r)
{
    return l.visitor() == r.visitor();
}

template <class ForwardVs1, class ForwardVs2>
inline Bool operator!=(const LeafIterator<ForwardVs1>& l,
    const LeafIterator<ForwardVs2>& r)
{
    return !(l == r);
}

template <class ForwardVs>
inline auto preBegin(ForwardVs root)
{
    return PreIterator<ForwardVs>::begin(root);
}

template <class ForwardVs>
inline auto preEnd(ForwardVs root)
{
    return PreIterator<ForwardVs>::end(root);
}

template <class ForwardVs>
inline auto postBegin(ForwardVs root)
{
    return PostIterator<ForwardVs>::begin(root);
}

template <class ForwardVs>
inline auto postEnd(ForwardVs root)
{
    return PostIterator<ForwardVs>::end(root);
}

template <class ForwardVs>
inline auto childBegin(ForwardVs parent)
{
    return ChildIterator<ForwardVs>::begin(parent);
}

template <class ForwardVs>
inline auto childEnd(ForwardVs parent)
{
    return ChildIterator<ForwardVs>::end(parent);
}

template <class ParentVs>
inline auto parentBegin(ParentVs child)
{
    return ParentIterator<ParentVs>::begin(child);
}

template <class ParentVs>
inline auto parentEnd(ParentVs child)
{
    return ParentIterator<ParentVs>::end(child);
}

template <class ForwardVs>
inline auto leafBegin(ForwardVs root)
{
    return LeafIterator<ForwardVs>::begin(root);
}

template <class ForwardVs>
inline auto leafEnd(ForwardVs root)
{
    return LeafIterator<ForwardVs>::end(root);
}

template <class ForwardVs>
inline auto preIters(ForwardVs root)
{
    return std::make_pair(preBegin(root), preEnd(root));
}

template <class ForwardVs>
inline auto postIters(ForwardVs root)
{
    return std::make_pair(postBegin(root), postEnd(root));
}

template <class ForwardVs>
inline auto childIters(ForwardVs parent)
{
    return std::make_pair(childBegin(parent), childEnd(parent));
}

template <class ParentVs>
inline auto parentIters(ParentVs child)
{
    return std::make_pair(parentBegin(child), parentEnd(child));
}

template <class ForwardVs>
inline auto leafIters(ForwardVs root)
{
    return std::make_pair(leafBegin(root), leafEnd(root));
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ITER_H_ */