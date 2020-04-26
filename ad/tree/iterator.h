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
namespace _iterator
{

struct PreTag {};
struct PostTag {};
struct ChildTag {};
struct ParentTag {};
struct LeafTag {};

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
ForwardVs begin(ForwardVs root, PreTag)
{
    return root;
}

template <class ForwardVs>
ForwardVs next(ForwardVs v, PreTag)
{
    if (v.first()) {
        return v.first();
    } else {
        return rightLowestAncestor(v);
    }
}

template <class BidirVs>
BidirVs prev(BidirVs v, PreTag)
{
    if (v.left()) {
        return rightLowestDescendant(v.left());
    } else {
        return v.parent();
    }
}

template <class ForwardVs>
ForwardVs end(ForwardVs root, PreTag)
{
    return rightLowestAncestor(root);
}

template <class ForwardVs>
ForwardVs last(ForwardVs root, PreTag)
{
    return root ? rightLowestDescendant(root) : root;
}

template <class ForwardVs>
ForwardVs begin(ForwardVs root, PostTag)
{
    return root ? leftLowestDescendant(root) : root;
}

template <class ForwardVs>
ForwardVs next(ForwardVs v, PostTag)
{
    if (v.right()) {
        return leftLowestDescendant(v.right());
    } else {
        return v.parent();
    }
}

template <class BidirVs>
BidirVs prev(BidirVs v, PostTag)
{
    if (v.last()) {
        return v.last();
    } else {
        return leftLowestAncestor(v);
    }
}

template <class ForwardVs>
ForwardVs end(ForwardVs root, PostTag)
{
    return root ? next(root, PostTag{}) : root;
}

template <class ForwardVs>
ForwardVs last(ForwardVs root, PostTag)
{
    return root;
}

template <class ForwardVs>
ForwardVs begin(ForwardVs parent, ChildTag)
{
    return parent.first();
}

template <class ForwardVs>
ForwardVs next(ForwardVs v, ChildTag)
{
    return v.right();
}

template <class BidirVs>
BidirVs prev(BidirVs v, ChildTag)
{
    return v.left();
}

template <class ForwardVs>
ForwardVs end(ForwardVs parent, ChildTag)
{
    return ForwardVs{};
}

template <class ForwardVs>
ForwardVs last(ForwardVs parent, ChildTag)
{
    return parent.last();
}

template <class ParentVs>
ParentVs begin(ParentVs child, ParentTag)
{
    return child.parent();
}

template <class ParentVs>
ParentVs next(ParentVs v, ParentTag)
{
    return v.parent();
}

template <class ParentVs>
ParentVs end(ParentVs child, ParentTag)
{
    return ParentVs{};
}

template <class ForwardVs>
ForwardVs begin(ForwardVs root, LeafTag)
{
    return root ? leftLowestDescendant(root) : root;
}

template <class ForwardVs>
ForwardVs next(ForwardVs v, LeafTag)
{
    v = rightLowestAncestor(v);
    return v ? leftLowestDescendant(v) : v;
}

template <class BidirVs>
BidirVs prev(BidirVs v, LeafTag)
{
    v = leftLowestAncestor(v);
    return v ? rightLowestDescendant(v) : v;
}

template <class ForwardVs>
ForwardVs end(ForwardVs root, LeafTag)
{
    return root ? next(root, LeafTag{}) : root;
}

template <class ForwardVs>
ForwardVs last(ForwardVs root, LeafTag)
{
    return root ? rightLowestDescendant(root) : root;
}

template <class Visitor, class Tag>
class ForwardIterator
{
    using IterType              = ForwardIterator<Visitor, Tag>;
    using VsTraits              = VisitorTraits<Visitor>;

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

    ForwardIterator()
        : mCur()
    {
    }

    template <class Visitor_>
    ForwardIterator(const ForwardIterator<Visitor_, Tag>& other)
        : mCur(other.visitor())
    {
    }

    template <class Visitor_>
    IterType& operator=(const ForwardIterator<Visitor_, Tag>& other)
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
        mCur = next(mCur, Tag{});
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
        return IterType(_iterator::begin(v, Tag{}));
    }

    inline static IterType end(VisitorType v)
    {
        return IterType(_iterator::end(v, Tag{}));
    }

private:
    explicit ForwardIterator(VisitorType visitor)
        : mCur(visitor)
    {
    }

    VisitorType      mCur;

}; /* class ForwardIterator */

template <class Visitor1, class Visitor2, class Tag>
inline Bool operator==(const ForwardIterator<Visitor1, Tag>& l,
    const ForwardIterator<Visitor2, Tag>& r)
{
    return l.visitor() == r.visitor();
}

template <class Visitor1, class Visitor2, class Tag>
inline Bool operator!=(const ForwardIterator<Visitor1, Tag>& l,
    const ForwardIterator<Visitor2, Tag>& r)
{
    return !(l == r);
}

template <class Visitor, class Tag>
class BidirIterator
{
    using IterType              = BidirIterator<Visitor, Tag>;
    using VsTraits              = VisitorTraits<Visitor>;

    template <class Visitor_, class Tag_> friend class BidirIterator;

public:
    using VisitorType           = typename VsTraits::VisitorType;
    using IteratorCategory      = std::bidirectional_iterator_tag;
    using ValueType             = typename VsTraits::ValueType;
    using DifferenceType        = typename VsTraits::DifferenceType;
    using Pointer               = typename VsTraits::Pointer;
    using Reference             = typename VsTraits::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    BidirIterator()
        : mCur(), mEnd(true)
    {
    }

    template <class Visitor_>
    BidirIterator(const BidirIterator<Visitor_, Tag>& other)
        : mCur(other.visitor()), mEnd(other.isEnd())
    {
    }

    template <class Visitor_>
    IterType& operator=(const BidirIterator<Visitor_, Tag>& other)
    {
        mCur = other.visitor();
        mEnd = other.isEnd();
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
        auto tmp = next(mCur, Tag{});
        if (!tmp) {
            mEnd = true;
        } else {
            mCur = tmp;
        }
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    IterType& operator--()
    {
        if (mEnd) {
            mEnd = false;
        } else {
            mCur = prev(mCur, Tag{});
        }
        return *this;
    }

    IterType operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    inline static IterType begin(VisitorType v)
    {
        auto b = _iterator::begin(v, Tag{});
        return b ? IterType(b, false) : IterType(b, true);
    }

    inline static IterType end(VisitorType v)
    {
        auto e = _iterator::end(v, Tag{});
        return e ? IterType(e, false) : IterType(_iterator::last(v, Tag{}), true);
    }

    template <class Visitor1, class Visitor2, class Tag_>
    friend inline Bool operator==(const BidirIterator<Visitor1, Tag_>& l,
        const BidirIterator<Visitor2, Tag_>& r)
    {
        return l.visitor() == r.visitor() && l.isEnd() == r.isEnd();
    }

    template <class Visitor1, class Visitor2, class Tag_>
    friend inline Bool operator!=(const BidirIterator<Visitor1, Tag_>& l,
        const BidirIterator<Visitor2, Tag_>& r)
    {
        return !(l == r);
    }

private:
    explicit BidirIterator(VisitorType visitor, bool end)
        : mCur(visitor), mEnd(end)
    {
    }

    Bool isEnd() const
    {
        return mEnd;
    }

    VisitorType      mCur;
    Bool             mEnd;

}; /* class BidirIterator */

} /* namespace _iterator */

template <class ForwardVs>
using PreIterator = _iterator::ForwardIterator<ForwardVs, _iterator::PreTag>;

template <class BidirVs>
using BidirPreIterator = _iterator::BidirIterator<BidirVs, _iterator::PreTag>;

template <class ForwardVs>
using PostIterator = _iterator::ForwardIterator<ForwardVs, _iterator::PostTag>;

template <class BidirVs>
using BidirPostIterator = _iterator::BidirIterator<BidirVs, _iterator::PostTag>;

template <class ForwardVs>
using ChildIterator = _iterator::ForwardIterator<ForwardVs, _iterator::ChildTag>;

template <class BidirVs>
using BidirChildIterator = _iterator::BidirIterator<BidirVs, _iterator::ChildTag>;

template <class ParentVs>
using ParentIterator = _iterator::ForwardIterator<ParentVs, _iterator::ParentTag>;

template <class ForwardVs>
using LeafIterator = _iterator::ForwardIterator<ForwardVs, _iterator::LeafTag>;

template <class BidirVs>
using BidirLeafIterator = _iterator::BidirIterator<BidirVs, _iterator::LeafTag>;

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

template <class BidirVs>
inline auto bidirPreBegin(BidirVs root)
{
    return BidirPreIterator<BidirVs>::begin(root);
}

template <class BidirVs>
inline auto bidirPreEnd(BidirVs root)
{
    return BidirPreIterator<BidirVs>::end(root);
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

template <class BidirVs>
inline auto bidirPostBegin(BidirVs root)
{
    return BidirPostIterator<BidirVs>::begin(root);
}

template <class BidirVs>
inline auto bidirPostEnd(BidirVs root)
{
    return BidirPostIterator<BidirVs>::end(root);
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

template <class BidirVs>
inline auto bidirChildBegin(BidirVs parent)
{
    return BidirChildIterator<BidirVs>::begin(parent);
}

template <class BidirVs>
inline auto bidirChildEnd(BidirVs parent)
{
    return BidirChildIterator<BidirVs>::end(parent);
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

template <class BidirVs>
inline auto bidirLeafBegin(BidirVs root)
{
    return BidirLeafIterator<BidirVs>::begin(root);
}

template <class BidirVs>
inline auto bidirLeafEnd(BidirVs root)
{
    return BidirLeafIterator<BidirVs>::end(root);
}

template <class ForwardVs>
inline auto preIters(ForwardVs root)
{
    return std::make_pair(preBegin(root), preEnd(root));
}

template <class BidirVs>
inline auto bidirPreIters(BidirVs root)
{
    return std::make_pair(bidirPreBegin(root), bidirPreEnd(root));
}

template <class ForwardVs>
inline auto postIters(ForwardVs root)
{
    return std::make_pair(postBegin(root), postEnd(root));
}

template <class BidirVs>
inline auto bidirPostIters(BidirVs root)
{
    return std::make_pair(bidirPostBegin(root), bidirPostEnd(root));
}

template <class ForwardVs>
inline auto childIters(ForwardVs parent)
{
    return std::make_pair(childBegin(parent), childEnd(parent));
}

template <class BidirVs>
inline auto bidirChildIters(BidirVs parent)
{
    return std::make_pair(bidirChildBegin(parent), bidirChildEnd(parent));
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

template <class BidirVs>
inline auto bidirLeafIters(BidirVs root)
{
    return std::make_pair(bidirLeafBegin(root), bidirLeafEnd(root));
}

namespace _iterator
{

template <class Visitor, class Tag>
struct ForwardView
{
    Visitor mVisitor;
};

template <class Visitor, class Tag>
auto begin(ForwardView<Visitor, Tag> forwardView)
{
    return ForwardIterator<Visitor, Tag>::begin(forwardView.mVisitor);
}

template <class Visitor, class Tag>
auto end(ForwardView<Visitor, Tag> forwardView)
{
    return ForwardIterator<Visitor, Tag>::end(forwardView.mVisitor);
}

template <class Visitor, class Tag>
struct BidirView
{
    Visitor mVisitor;
};

template <class Visitor, class Tag>
auto begin(BidirView<Visitor, Tag> bidirView)
{
    return BidirIterator<Visitor, Tag>::begin(bidirView.mVisitor);
}

template <class Visitor, class Tag>
auto end(BidirView<Visitor, Tag> bidirView)
{
    return BidirIterator<Visitor, Tag>::end(bidirView.mVisitor);
}

} /* namespace _iterator */

template <class Visitor>
auto preView(Visitor root)
{
    return _iterator::ForwardView<Visitor, PreTag>();
}

template <class Visitor>
auto postView(Visitor root)
{
    return _iterator::ForwardView<Visitor, PostTag>();
}

template <class Visitor>
auto childView(Visitor root)
{
    return _iterator::ForwardView<Visitor, ChildTag>();
}

template <class Visitor>
auto parentView(Visitor root)
{
    return _iterator::ForwardView<Visitor, ParentTag>();
}

template <class Visitor>
auto leafView(Visitor root)
{
    return _iterator::ForwardView<Visitor, LeafTag>();
}

template <class Visitor>
auto bidirPreView(Visitor root)
{
    return _iterator::BidirView<Visitor, PreTag>();
}

template <class Visitor>
auto bidirPostView(Visitor root)
{
    return _iterator::BidirView<Visitor, PostTag>();
}

template <class Visitor>
auto bidirChildView(Visitor root)
{
    return _iterator::BidirView<Visitor, ChildTag>();
}

template <class Visitor>
auto bidirParentView(Visitor root)
{
    return _iterator::BidirView<Visitor, ParentTag>();
}

template <class Visitor>
auto bidirLeafView(Visitor root)
{
    return _iterator::BidirView<Visitor, LeafTag>();
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ITER_H_ */