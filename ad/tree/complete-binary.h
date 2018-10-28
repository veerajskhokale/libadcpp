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

#ifndef AD_TREE_COMPLETE_BINARY_H_
#define AD_TREE_COMPLETE_BINARY_H_

#include "ad/types.h"
#include "ad/tree/iterator.h"
#include <vector>
#include <utility>
#include <initializer_list>

namespace ad
{
namespace tree
{
namespace
{

const Size NULL_INDEX = 0;

template <class RandomIt>
class TreeVisitor
{
    using VisitorType           = TreeVisitor<RandomIt>;
    using IteratorType          = RandomIt;

    template <class, class> friend class CompleteBinaryTree;
    template <class, class> friend class TreeConstVisitor;

public:
    using ValueType             = typename IteratorType::value_type;
    using Reference             = typename IteratorType::reference;
    using Pointer               = typename IteratorType::pointer;
    using DifferenceType        = typename IteratorType::difference_type;

    TreeVisitor()
        : mFirst(), mLast(), mIdx(NULL_INDEX)
    {
    }

    TreeVisitor(const VisitorType& visitor)
        : mFirst(visitor.mFirst), mLast(visitor.mLast), mIdx(visitor.mIdx)
    {
    }

    Reference operator*() const
    {
        return mFirst[mIdx - 1];
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to(mFirst[mIdx - 1]);
    }

    VisitorType parent() const
    {
        return VisitorType(mFirst, mLast, mIdx >> 1);
    }

    VisitorType first() const
    {
        return VisitorType(mFirst, mLast, mIdx << 1);
    }

    VisitorType last() const
    {
        return VisitorType(mFirst, mLast, (mIdx << 1) + 1);
    }

    VisitorType left() const
    {
        return !(mIdx & 1) || (mIdx == 1) ?
            VisitorType(mFirst, mLast, NULL_INDEX) :
            VisitorType(mFirst, mLast, mIdx - 1);
    }

    VisitorType right() const
    {
        return (mIdx & 1) ?
            VisitorType(mFirst, mLast, NULL_INDEX) :
            VisitorType(mFirst, mLast, mIdx + 1);
    }

    explicit operator Bool() const
    {
        return mIdx != NULL_INDEX && mIdx <= (mLast - mFirst);
    }

    inline friend Bool operator==(const VisitorType& l,
        const VisitorType& r)
    {
        return (l.mFirst == r.mFirst) && (l.mLast == r.mLast) && (l.mIdx == r.mIdx);
    }

    inline friend Bool operator!=(const VisitorType& l,
        const VisitorType& r)
    {
        return !(l == r);
    }

private:
    TreeVisitor(IteratorType first, IteratorType last, Size idx)
        : mFirst(first), mLast(last), mIdx(idx)
    {
    }

    IteratorType    mFirst;
    IteratorType    mLast;
    DifferenceType  mIdx;

}; /* class TreeVisitor */

template <class ConstRandomIt, class RandomIt>
class TreeConstVisitor
{
    using ConstVisitorType      = TreeConstVisitor<ConstRandomIt, RandomIt>;
    using VisitorType           = TreeVisitor<RandomIt>;
    using ConstIteratorType     = ConstRandomIt;
    using IteratorType          = RandomIt;

    template <class, class> friend class CompleteBinaryTree;

public:
    using ValueType             = typename ConstIteratorType::value_type;
    using Reference             = typename ConstIteratorType::reference;
    using Pointer               = typename ConstIteratorType::pointer;
    using DifferenceType        = typename ConstIteratorType::difference_type;

    TreeConstVisitor()
        : mFirst(), mLast(), mIdx(NULL_INDEX)
    {
    }

    TreeConstVisitor(const VisitorType& visitor)
        : mFirst(visitor.mFirst), mLast(visitor.mLast), mIdx(visitor.mIdx)
    {
    }

    TreeConstVisitor(const ConstVisitorType& visitor)
        : mFirst(visitor.mFirst), mLast(visitor.mLast), mIdx(visitor.mIdx)
    {
    }

    ConstVisitorType& operator=(const VisitorType& visitor)
    {
        mFirst = visitor.mFirst;
        mLast = visitor.mLast;
        mIdx = visitor.mIdx;
        return *this;
    }

    ConstVisitorType& operator=(const ConstVisitorType& visitor)
    {
        mFirst = visitor.mFirst;
        mLast = visitor.mLast;
        mIdx = visitor.mIdx;
        return *this;
    }

    Reference operator*() const
    {
        return mFirst[mIdx - 1];
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to(mFirst[mIdx - 1]);
    }

    ConstVisitorType parent() const
    {
        return ConstVisitorType(mFirst, mLast, mIdx >> 1);
    }

    ConstVisitorType first() const
    {
        return ConstVisitorType(mFirst, mLast, mIdx << 1);
    }

    ConstVisitorType last() const
    {
        return ConstVisitorType(mFirst, mLast, (mIdx << 1) + 1);
    }

    ConstVisitorType left() const
    {
        return !(mIdx & 1) || (mIdx == 1) ?
            ConstVisitorType(mFirst, mLast, NULL_INDEX) :
            ConstVisitorType(mFirst, mLast, mIdx - 1);
    }

    ConstVisitorType right() const
    {
        return (mIdx & 1) ?
            ConstVisitorType(mFirst, mLast, NULL_INDEX) :
            ConstVisitorType(mFirst, mLast, mIdx + 1);
    }

    explicit operator Bool() const
    {
        return mIdx != NULL_INDEX && mIdx <= (mLast - mFirst);
    }

    inline friend Bool operator==(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return (l.mFirst == r.mFirst) && (l.mLast == r.mLast) && (l.mIdx == r.mIdx);
    }

    inline friend Bool operator!=(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return !(l == r);
    }

private:
    TreeConstVisitor(ConstIteratorType first, ConstIteratorType last, Size idx)
        : mFirst(first), mLast(last), mIdx(idx)
    {
    }

    ConstIteratorType   mFirst;
    ConstIteratorType   mLast;
    DifferenceType      mIdx;

}; /* class TreeConstVisitor */

} /* namespace anonymous */

template <class Val, class Alloc = std::allocator<Val>>
class CompleteBinaryTree
{
    using TreeType                  = CompleteBinaryTree<Val, Alloc>;

public:
    using AllocatorType             = Alloc;
    using ValueType                 = Val;

private:
    using VectorType                = std::vector<ValueType, AllocatorType>;

public:
    using Reference                 = typename VectorType::reference;
    using ConstReference            = typename VectorType::const_reference;
    using Pointer                   = typename VectorType::pointer;
    using ConstPointer              = typename VectorType::const_pointer;
    using SizeType                  = typename VectorType::size_type;
    using DifferenceType            = typename VectorType::difference_type;

    using allocator_type            = AllocatorType;
    using value_type                = ValueType;
    using reference                 = Reference;
    using const_reference           = ConstReference;
    using pointer                   = Pointer;
    using const_pointer             = ConstPointer;
    using size_type                 = SizeType;
    using difference_type           = DifferenceType;

private:
    using VectorIterator            = typename VectorType::iterator;
    using VectorConstIterator       = typename VectorType::const_iterator;

public:
    using Visitor                   = TreeVisitor<VectorIterator>;
    using ConstVisitor              = TreeConstVisitor<VectorConstIterator, VectorIterator>;

    CompleteBinaryTree()
        : mVec()
    {
    }

    explicit CompleteBinaryTree(const AllocatorType& alloc)
        : mVec(alloc)
    {
    }

    explicit CompleteBinaryTree(SizeType count,
        ConstReference val = ValueType(),
        const AllocatorType& alloc = AllocatorType())
        : mVec(count, val, alloc)
    {
    }

    template <class Visitor_, typename = std::enable_if_t<
        !std::is_base_of<TreeType, std::decay_t<Visitor_>>::value &&
            !std::is_convertible<SizeType, std::decay_t<Visitor_>>::value
        >
    >
    explicit CompleteBinaryTree(Visitor_ visitor,
        const AllocatorType& alloc = AllocatorType())
        : mVec(PreIterator<Visitor_>::begin(visitor),
            PreIterator<Visitor_>::end(visitor), alloc)
    {
    }

    CompleteBinaryTree(const TreeType& tree)
        : mVec(tree.mVec)
    {
    }

    CompleteBinaryTree(TreeType&& tree)
        : mVec(std::move(tree.mVec))
    {
    }

    Void clear()
    {
        mVec.clear();
    }

    ~CompleteBinaryTree()
    {
        clear();
    }

    TreeType& operator=(const TreeType& tree)
    {
        mVec = tree.mVec;
        return *this;
    }

    TreeType& operator=(TreeType&& tree)
    {
        mVec = std::move(tree.mVec);
        return *this;
    }

    Bool empty() const
    {
        return mVec.empty();
    }

    SizeType size() const
    {
        return mVec.size();
    }

    SizeType maxSize() const
    {
        return mVec.max_size();
    }

    SizeType max_size() const
    {
        return maxSize();
    }

    Visitor root()
    {
        return Visitor(mVec.begin(), mVec.end(), 1);
    }

    ConstVisitor root() const
    {
        return ConstVisitor(mVec.begin(), mVec.end(), 1);
    }

    ConstVisitor croot() const
    {
        return ConstVisitor(mVec.cbegin(), mVec.cend(), 1);
    }

    AllocatorType getAllocator() const
    {
        return mVec.get_allocator();
    }

    AllocatorType get_allocator() const
    {
        return getAllocator();
    }

    Void resize(SizeType count, ConstReference val)
    {
        mVec.resize(count, val);
    }

    Void assign(SizeType count, ConstReference val)
    {
        mVec.assign(count, val);
    }

    template <class Visitor_>
    Void assign(Visitor_ root)
    {
        mVec.assign(PreIterator<Visitor_>::begin(root),
            PreIterator<Visitor_>::end(root));
    }

    Void assign(std::initializer_list<ValueType> initList)
    {
        mVec.assign(initList);
    }

    Void pushBack(ConstReference val)
    {
        mVec.push_back(val);
    }

    Void pushBack(ValueType&& val)
    {
        mVec.push_back(std::move(val));
    }

    template <class... Args>
    Void emplaceBack(Args&&... args)
    {
        mVec.push_back(std::forward<Args>(args)...);
    }

    Void popBack()
    {
        mVec.pop_back();
    }

    Reference operator[](SizeType pos)
    {
        return mVec[pos];
    }

    ConstReference operator[](SizeType pos) const
    {
        return mVec[pos];
    }

private:
    VectorType      mVec;

}; /* class CompleteBinaryTree */

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_COMPLETE_BINARY_H_ */