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

#ifndef AD_TREE_COMPLETE_BINARY_TREE_H_
#define AD_TREE_COMPLETE_BINARY_TREE_H_

#include "ad/types.h"
#include "ad/tree/visitor.h"
#include "ad/tree/iterator.h"
#include <vector>
#include <utility>
#include <initializer_list>

namespace ad
{
namespace tree
{

template <class Tree, class ConstVisitor>
class CBTreeVisitor
{
    using VisitorType           = CBTreeVisitor<Tree, ConstVisitor>;
    using TreeType              = Tree;

    friend Tree;
    friend ConstVisitor;

public:
    using VisitorCategory       = CompleteBinaryVisitorTag;
    using ValueType             = typename TreeType::ValueType;
    using Reference             = typename TreeType::Reference;
    using Pointer               = typename TreeType::Pointer;
    using DifferenceType        = typename TreeType::DifferenceType;

    CBTreeVisitor()
        : mTree(nullptr), mIdx(0)
    {
    }

    CBTreeVisitor(const VisitorType& visitor) = default;

    VisitorType& operator=(const VisitorType& visitor) = default;

    Reference operator*() const
    {
        return (*mTree)[mIdx - 1];
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to((*mTree)[mIdx - 1]);
    }

    VisitorType parent() const
    {
        return VisitorType(mTree, mIdx >> 1);
    }

    VisitorType first() const
    {
        return VisitorType(mTree, mIdx << 1);
    }

    VisitorType last() const
    {
        return VisitorType(mTree, (mIdx << 1) + 1);
    }

    VisitorType left() const
    {
        return !(mIdx & 1) || (mIdx == 1) ?
            VisitorType(nullptr, 0) : VisitorType(mTree, mIdx - 1);
    }

    VisitorType right() const
    {
        return (mIdx & 1) ? VisitorType(nullptr, 0)
            : VisitorType(mTree, mIdx + 1);
    }

    VisitorType leftChild() const
    {
        return first();
    }

    VisitorType rightChild() const
    {
        return last();
    }

    explicit operator Bool() const
    {
        return mIdx && mIdx <= (DifferenceType)mTree->size();
    }

    inline friend Bool operator==(const VisitorType& l,
        const VisitorType& r)
    {
        return (l.mIdx == r.mIdx || (!l && !r));
    }

    inline friend Bool operator!=(const VisitorType& l,
        const VisitorType& r)
    {
        return !(l == r);
    }

private:
    CBTreeVisitor(TreeType* tree, DifferenceType idx)
        : mTree(tree), mIdx(idx)
    {
    }

    TreeType*       mTree;
    DifferenceType  mIdx;

}; /* class CBTreeVisitor */

template <class Tree>
class CBTreeConstVisitor
{
    using ConstVisitorType      = CBTreeConstVisitor<Tree>;
    using VisitorType           = CBTreeVisitor<Tree, ConstVisitorType>;
    using TreeType              = Tree;

    friend Tree;

public:
    using VisitorCategory       = CompleteBinaryVisitorTag;
    using ValueType             = typename TreeType::ValueType;
    using Reference             = typename TreeType::ConstReference;
    using Pointer               = typename TreeType::ConstPointer;
    using DifferenceType        = typename TreeType::DifferenceType;

    CBTreeConstVisitor()
        : mTree(nullptr), mIdx(0)
    {
    }

    CBTreeConstVisitor(const ConstVisitorType& visitor) = default;

    CBTreeConstVisitor(const VisitorType& visitor)
        : mTree(visitor.mTree), mIdx(visitor.mIdx)
    {
    }

    ConstVisitorType& operator=(const ConstVisitorType& visitor) = default;

    ConstVisitorType& operator=(const VisitorType& visitor)
    {
        mTree = visitor.mTree;
        mIdx = visitor.mIdx;
    }

    Reference operator*() const
    {
        return (*mTree)[mIdx - 1];
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to((*mTree)[mIdx - 1]);
    }

    ConstVisitorType parent() const
    {
        return ConstVisitorType(mTree, mIdx >> 1);
    }

    ConstVisitorType first() const
    {
        return ConstVisitorType(mTree, mIdx << 1);
    }

    ConstVisitorType last() const
    {
        return ConstVisitorType(mTree, (mIdx << 1) + 1);
    }

    ConstVisitorType left() const
    {
        return !(mIdx & 1) || (mIdx == 1) ?
            ConstVisitorType(nullptr, 0) : ConstVisitorType(mTree, mIdx - 1);
    }

    ConstVisitorType right() const
    {
        return (mIdx & 1) ? ConstVisitorType(nullptr, 0) :
            ConstVisitorType(mTree, mIdx + 1);
    }

    ConstVisitorType leftChild() const
    {
        return first();
    }

    ConstVisitorType rightChild() const
    {
        return last();
    }

    explicit operator Bool() const
    {
        return mIdx && mIdx <= (DifferenceType)mTree->size();
    }

    inline friend Bool operator==(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return l.mTree == r.mTree && (l.mIdx == r.mIdx || (!l && !r));
    }

    inline friend Bool operator!=(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return !(l == r);
    }

private:
    CBTreeConstVisitor(const TreeType* tree, DifferenceType idx)
        : mTree(tree), mIdx(idx)
    {
    }

    const TreeType*     mTree;
    DifferenceType      mIdx;

}; /* class CBTreeConstVisitor */

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

    using ConstVisitor              = CBTreeConstVisitor<TreeType>;
    using Visitor                   = CBTreeVisitor<TreeType, ConstVisitor>;

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
        return Visitor(this, 1);
    }

    ConstVisitor root() const
    {
        return ConstVisitor(this, 1);
    }

    ConstVisitor croot() const
    {
        return root();
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

#endif /* AD_TREE_COMPLETE_BINARY_TREE_H_ */