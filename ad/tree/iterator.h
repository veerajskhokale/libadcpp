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

namespace ad
{
namespace tree
{

template <class Visitor>
Visitor leftLowestDescendant(Visitor v)
{
    for (; v.first(); v = v.first());
    return v;
}

template <class Visitor>
Visitor rightLowestDescendant(Visitor v)
{
    for (; v.last(); v = v.last());
    return v;
}

template <class Visitor>
Visitor preNext(Visitor v)
{
    if (v.first()) {
        return v.first();
    } else {
        for (; v && !v.right(); v = v.parent());
        return v ? v.right() : v;
    }
}

template <class Visitor>
Visitor prePrev(Visitor v)
{
    if (v.left()) {
        return rightLowestDescendant(v.left());
    } else {
        return v.parent();
    }
}

template <class Visitor>
Visitor postNext(Visitor v)
{
    if (v.right()) {
        return leftLowestDescendant(v.right());
    } else {
        return v.parent();
    }
}

template <class Visitor>
Visitor postPrev(Visitor v)
{
    if (v.last()) {
        return v.last();
    } else {
        for (; v && !v.left(); v = v.parent());
        return v ? v.left() : v;
    }
}

template <class Visitor>
class PreIterator
{
    using IterType              = PreIterator<Visitor>;

public:
    using VisitorType           = Visitor;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VisitorType::ValueType;
    using DifferenceType        = typename VisitorType::DifferenceType;
    using Pointer               = typename VisitorType::Pointer;
    using Reference             = typename VisitorType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PreIterator()
        : mCur()
    {
    }

    template <class Visitor2>
    PreIterator(const PreIterator<Visitor2>& other)
        : mCur(other.visitor())
    {
    }

    template <class Visitor2>
    IterType& operator=(const PreIterator<Visitor2>& other)
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
        return v ? IterType(preNext(rightLowestDescendant(v))) :
            IterType(v);
    }

private:
    explicit PreIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class PreIterator */

template <class Visitor1, class Visitor2>
inline Bool operator==(const PreIterator<Visitor1>& l,
    const PreIterator<Visitor2>& r)
{
    return l.visitor() == r.visitor();
}

template <class Visitor1, class Visitor2>
inline Bool operator!=(const PreIterator<Visitor1>& l,
    const PreIterator<Visitor2>& r)
{
    return !(l == r);
}

template <class Visitor>
class PostIterator
{
    using IterType              = PostIterator<Visitor>;

public:
    using VisitorType           = Visitor;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VisitorType::ValueType;
    using DifferenceType        = typename VisitorType::DifferenceType;
    using Pointer               = typename VisitorType::Pointer;
    using Reference             = typename VisitorType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PostIterator()
        : mCur()
    {
    }

    template <class Visitor2>
    PostIterator(const PostIterator<Visitor2>& other)
        : mCur(other.visitor())
    {
    }

    template <class Visitor2>
    IterType& operator=(const PostIterator<Visitor2>& other)
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

template <class Visitor1, class Visitor2>
inline Bool operator==(const PostIterator<Visitor1>& l,
    const PostIterator<Visitor2>& r)
{
    return l.visitor() == r.visitor();
}

template <class Visitor1, class Visitor2>
inline Bool operator!=(const PostIterator<Visitor1>& l,
    const PostIterator<Visitor2>& r)
{
    return !(l == r);
}

template <class Visitor>
class ChildIterator
{
    using IterType              = ChildIterator<Visitor>;

public:
    using VisitorType           = Visitor;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VisitorType::ValueType;
    using DifferenceType        = typename VisitorType::DifferenceType;
    using Pointer               = typename VisitorType::Pointer;
    using Reference             = typename VisitorType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    ChildIterator()
        : mCur()
    {
    }

    template <class Visitor2>
    ChildIterator(const ChildIterator<Visitor2>& other)
        : mCur(other.visitor())
    {
    }

    template <class Visitor2>
    IterType& operator=(const ChildIterator<Visitor2>& other)
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
        return v ? (v.last() ? IterType(v.last().right()) : IterType(v.last()))
                    : IterType(v);
    }

private:
    explicit ChildIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class ChildIterator */

template <class Visitor1, class Visitor2>
inline Bool operator==(const ChildIterator<Visitor1>& l,
    const ChildIterator<Visitor2>& r)
{
    return l.visitor() == r.visitor();
}

template <class Visitor1, class Visitor2>
inline Bool operator!=(const ChildIterator<Visitor1>& l,
    const ChildIterator<Visitor2>& r)
{
    return !(l == r);
}

template <class Visitor, class = Void>
struct GetVisitor
{
    using VisitorType = Visitor;

    VisitorType get(Visitor v) const
    {
        return v;
    }
};

template <class Iterator>
struct GetVisitor<Iterator, mp::enableIfType_<typename Iterator::VisitorType>>
{
    using VisitorType = typename Iterator::VisitorType;

    VisitorType get(Iterator iter) const
    {
        return iter.visitor();
    }
};

template <class Visitor>
inline typename GetVisitor<Visitor>::VisitorType getVisitor(Visitor v)
{
    return GetVisitor<Visitor>().get(v);
}

template <class Visitor>
inline auto preBegin(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return PreIterator<VisitorType>::begin(getVisitor(v));
}

template <class Visitor>
inline auto preEnd(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return PreIterator<VisitorType>::end(getVisitor(v));
}

template <class Visitor>
inline auto postBegin(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return PostIterator<VisitorType>::begin(getVisitor(v));
}

template <class Visitor>
inline auto postEnd(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return PostIterator<VisitorType>::end(getVisitor(v));
}

template <class Visitor>
inline auto childBegin(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return ChildIterator<VisitorType>::begin(getVisitor(v));
}

template <class Visitor>
inline auto childEnd(Visitor v)
{
    using VisitorType = typename GetVisitor<Visitor>::VisitorType;
    return ChildIterator<VisitorType>::end(getVisitor(v));
}

} /* namespace tree*/
} /* namespace ad */

#endif /* AD_TREE_ITER_H_ */