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

#ifndef AD_TREE_GENERIC_H_
#define AD_TREE_GENERIC_H_

#include <memory>
#include <type_traits>
#include <iterator>
#include <algorithm>
#include "ad/types.h"
#include "ad/tree/iter.h"

namespace ad
{
namespace tree
{
namespace
{

template <class Val, class VoidPtr>
class TreeNode
{
    using NodeType      = TreeNode<Val, VoidPtr>;

public:
    using ValueType     = Val;
    using Reference     = Val&;
    using RawPtr        = Val*;
    using NodePtr       = typename std::pointer_traits<VoidPtr>::
        template rebind<NodeType>;

    RawPtr valRawPtr()
    {
        return std::addressof(mVal);
    }

    Reference val()
    {
        return mVal;
    }

    NodePtr parent()
    {
        return mParent;
    }

    NodePtr first()
    {
        return mFirst;
    }

    NodePtr last()
    {
        return mLast;
    }

    NodePtr left()
    {
        return mLeft;
    }

    NodePtr right()
    {
        return mRight;
    }

    Void clear()
    {
        mParent = mFirst = mLast = mLeft = mRight = nullptr;
    }

    Void pushBack(NodePtr child)
    {
        if (mLast) {
            mLast->mRight = child;
            child->mLeft = mLast;
        } else {
            mFirst = child;
        }
        mLast = child;
        child->mParent = this;
    }

    Void pushFront(NodePtr child)
    {
        if (mFirst) {
            mFirst->mLeft = child;
            child->mRight = mFirst;
        } else {
            mLast = child;
        }
        mFirst = child;
        child->mParent = this;
    }

    Void insert(NodePtr child, NodePtr rightSibling)
    {
        if (!rightSibling) {
            pushBack(child);
        } else if (mFirst == rightSibling) {
            pushFront(child);
        } else {
            auto leftSibling = rightSibling->mLeft;
            leftSibling->mRight = child;
            rightSibling->mLeft = child;
            child->mRight = rightSibling;
            child->mLeft = leftSibling;
            child->mParent = this;
        }
    }

    NodePtr popBack()
    {
        auto child = mLast;
        if (!child) {
            return nullptr;
        }

        mLast = child->mLeft;
        if (!mLast) {
            mFirst = nullptr;
        } else {
            mLast->mRight = nullptr;
            child->mLeft = nullptr;
        }
        child->mParent = nullptr;
        return child;
    }

    NodePtr popFront()
    {
        auto child = mFirst;
        if (!child) {
            return nullptr;
        }

        mFirst = child->mRight;
        if (!mFirst) {
            mLast = nullptr;
        } else {
            mFirst->mLeft = nullptr;
            child->mRight = nullptr;
        }
        child->mParent = nullptr;
        return child;
    }

    NodePtr remove(NodePtr child)
    {
        if (child == mLast) {
            return popBack();
        } else if (child == mFirst) {
            return popFront();
        } else {
            child->mRight->mLeft = child->mLeft;
            child->mLeft->mRight = child->mRight;
            child->mLeft = child->mRight = child->mParent = nullptr;
            return child;
        }
    }

private:
    NodePtr     mParent;
    NodePtr     mFirst;
    NodePtr     mLast;
    NodePtr     mLeft;
    NodePtr     mRight;

    ValueType   mVal;

};

template <class Val, class VoidPtr>
class TreeCrawler
{
    using CrawlerType           = TreeCrawler<Val, VoidPtr>;
    using NodePtr               = typename TreeNode<Val, VoidPtr>::NodePtr;

    template <class, class> friend class TreeBase;
    template <class, class> friend class Tree;
    template <class, class> friend class TreeConstCrawler;

public:
    using ValueType             = Val;
    using Reference             = Val&;
    using Pointer               = typename std::pointer_traits<VoidPtr>::
        template rebind<ValueType>;
    using DifferenceType        = typename std::pointer_traits<Pointer>::
        difference_type;

    TreeCrawler()
        : mNode(nullptr)
    {
    }

    Reference operator*() const
    {
        return mNode->val();
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to(mNode->val());
    }

    CrawlerType& parent()
    {
        mNode = mNode->parent();
        return *this;
    }

    CrawlerType& first()
    {
        mNode = mNode->first();
        return *this;
    }

    CrawlerType& last()
    {
        mNode = mNode->last();
        return *this;
    }

    CrawlerType& left()
    {
        mNode = mNode->left();
        return *this;
    }

    CrawlerType& right()
    {
        mNode = mNode->right();
        return *this;
    }

    explicit operator Bool() const
    {
        return node() != nullptr;
    }

    inline friend Bool operator==(const CrawlerType& l,
        const CrawlerType& r)
    {
        return l.node() == r.node();
    }

    inline friend Bool operator!=(const CrawlerType& l,
        const CrawlerType& r)
    {
        return !(l == r);
    }

private:
    explicit TreeCrawler(NodePtr ptr)
        : mNode(ptr)
    {
    }

    NodePtr node() const
    {
        return mNode;
    }

    NodePtr     mNode;

};

template <class Val, class VoidPtr>
class TreeConstCrawler
{
    using ConstCrawlerType      = TreeConstCrawler<Val, VoidPtr>;
    using CrawlerType           = TreeCrawler<Val, VoidPtr>;
    using NodePtr               = typename TreeNode<Val, VoidPtr>::NodePtr;

    template <class, class> friend class TreeBase;
    template <class, class> friend class Tree;

public:
    using ValueType             = Val;
    using Reference             = const Val&;
    using Pointer               = typename std::pointer_traits<VoidPtr>::
        template rebind<const ValueType>;
    using DifferenceType        = typename std::pointer_traits<Pointer>::
        difference_type;

    TreeConstCrawler()
        : mNode(nullptr)
    {
    }

    TreeConstCrawler(const CrawlerType& crawler)
        : mNode(crawler.node())
    {
    }

    TreeConstCrawler(const ConstCrawlerType& crawler)
        : mNode(crawler.node())
    {
    }

    Reference operator*() const
    {
        return mNode->val();
    }

    Pointer operator->() const
    {
        return std::pointer_traits<Pointer>::
            pointer_to(mNode->val());
    }

    ConstCrawlerType& parent()
    {
        mNode = mNode->parent();
        return *this;
    }

    ConstCrawlerType& first()
    {
        mNode = mNode->first();
        return *this;
    }

    ConstCrawlerType& last()
    {
        mNode = mNode->last();
        return *this;
    }

    ConstCrawlerType& left()
    {
        mNode = mNode->left();
        return *this;
    }

    ConstCrawlerType& right()
    {
        mNode = mNode->right();
        return *this;
    }

    explicit operator Bool() const
    {
        return node() != nullptr;
    }

    inline friend Bool operator==(const ConstCrawlerType& l,
        const ConstCrawlerType& r)
    {
        return l.node() == r.node();
    }

    inline friend Bool operator!=(const ConstCrawlerType& l,
        const ConstCrawlerType& r)
    {
        return !(l == r);
    }

private:
    explicit TreeConstCrawler(NodePtr ptr)
        : mNode(ptr)
    {
    }

    NodePtr node() const
    {
        return mNode;
    }

    NodePtr     mNode;

};

template <class Val, class Alloc>
class TreeBase
{
    using TreeBaseType          = TreeBase<Val, Alloc>;

protected:
    using AllocatorType         = Alloc;
    using AllocTraits           = std::allocator_traits<AllocatorType>;
    using ValueType             = typename AllocTraits::value_type;
    using Reference             = ValueType&;
    using ConstReference        = const ValueType&;
    using Pointer               = typename AllocTraits::pointer;
    using ConstPointer          = typename AllocTraits::const_pointer;
    using SizeType              = typename AllocTraits::size_type;
    using DifferenceType        = typename AllocTraits::difference_type;

    using VoidPtr               = typename AllocTraits::void_pointer;
    using Node                  = TreeNode<Val, VoidPtr>;
    using NodeAlloc             = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits       = std::allocator_traits<NodeAlloc>;
    using NodePtr               = typename Node::NodePtr;

    using Crawler               = TreeCrawler<Val, VoidPtr>;
    using ConstCrawler          = TreeConstCrawler<Val, VoidPtr>;

    using PreIter               = PreIterator<Crawler>;
    using PostIter              = PostIterator<Crawler>;
    using ChildIter             = ChildIterator<Crawler>;

    using ConstPreIter          = PreIterator<ConstCrawler>;
    using ConstPostIter         = PostIterator<ConstCrawler>;
    using ConstChildIter        = ChildIterator<ConstCrawler>;

    TreeBase()
        : mNodeAl(), mRoot(nullptr)
    {
    }

    explicit TreeBase(const AllocatorType& alloc)
        : mNodeAl(alloc), mRoot(nullptr)
    {
    }

    TreeBase(const NodeAlloc& nodeAl, NodePtr root)
        : mNodeAl(nodeAl), mRoot(root)
    {
    }

    TreeBase(const TreeBase& tree)
        : mNodeAl(NodeAllocTraits::
            select_on_container_copy_construction(tree.nodeAlloc())),
            mRoot(copy(tree.root()))
    {
    }

    TreeBase(TreeBase&& tree)
        : mNodeAl(std::move(tree.nodeAlloc())),
            mRoot(tree.move(tree.rootNode()))
    {
        tree.clearRoot();
    }

    ~TreeBase()
    {
        clear();
    }

    TreeBaseType& operator=(const TreeBaseType& tree)
    {
        if (std::addressof(tree) == this) {
            return *this;
        }

        copyAssign(tree, NodeAllocTraits::
            propagate_on_container_copy_assignment);

        return *this;
    }

    TreeBaseType& operator=(TreeBaseType&& tree)
    {
        if (std::addressof(tree) == this) {
            return *this;
        }

        moveAssign(std::move(tree), NodeAllocTraits::
            propagate_on_container_move_assignment);

        return *this;
    }

    NodeAlloc& nodeAlloc()
    {
        return mNodeAl;
    }

    NodeAlloc nodeAlloc() const
    {
        return mNodeAl;
    }

    NodePtr& rootNode()
    {
        return mRoot;
    }

    NodePtr rootNode() const
    {
        return mRoot;
    }

    Crawler root()
    {
        return Crawler(rootNode());
    }

    ConstCrawler root() const
    {
        return ConstCrawler(rootNode());
    }

    Void clearRoot()
    {
        rootNode() = nullptr;
    }

    AllocatorType getAllocator() const
    {
        return AllocatorType(nodeAlloc());
    }

    template <class... Args>
    NodePtr createNode(Args&&... args)
    {
        auto ptr = NodeAllocTraits::allocate(nodeAlloc(), 1);
        NodeAllocTraits::construct(nodeAlloc(), ptr->valRawPtr(),
            std::forward<Args>(args)...);
        ptr->clear();
        return ptr;
    }

    Void destroyNode(NodePtr node)
    {
        NodeAllocTraits::destroy(nodeAlloc(), node->valRawPtr());
        NodeAllocTraits::deallocate(nodeAlloc(), node, 1);
    }

    template <class Crawler2>
    NodePtr copy(Crawler2 cw)
    {
        using SrcPreIter    = PreIterator<Crawler2>;
        using SrcChildIter  = ChildIterator<Crawler2>;

        if (!cw) {
            return nullptr;
        }

        auto desRoot = createNode(*cw);
        auto desParent = PreIter::begin(Crawler(desRoot));
        for (auto srcParent = SrcPreIter::begin(cw); srcParent !=
            SrcPreIter::end(cw); ++srcParent, ++desParent) {
            for (auto srcChild = SrcChildIter::begin(srcParent.crawler());
                srcChild != SrcChildIter::end(srcParent.crawler());
                ++srcChild) {
                auto desChild = createNode(*srcChild);
                desParent.crawler().node()->pushBack(desChild);
            }
        }
        return desRoot;
    }

    NodePtr move(NodePtr node)
    {
        if (!node) {
            return nullptr;
        }

        if (node->parent()) {
            node->parent()->remove(node);
        } else {
            clearRoot();
        }
        return node;
    }

    Void destroy(NodePtr node)
    {
        if (!node) {
            return;
        }

        if (node->parent()) {
            node->parent()->remove(node);
        } else {
            clearRoot();
        }

        for (auto parent = PostIter::begin(Crawler(node));
            parent != PostIter::end(Crawler(node)); ++parent) {
            while (auto child = parent.crawler().node()->popBack()) {
                destroyNode(child);
            }
        }
        destroyNode(node);
    }

    Void copyAssignAlloc(const NodeAlloc& nodeAl, std::true_type)
    {
        nodeAlloc() = nodeAl;
    }

    Void copyAssignAlloc(const NodeAlloc& nodeAl, std::false_type)
    {
    }

    template <Bool truth>
    Void copyAssign(const TreeBaseType& tree,
        std::integral_constant<Bool, truth>)
    {
        clear();
        copyAssignAlloc(tree.nodeAlloc(), truth);
        rootNode() = copy(tree.root());
    }

    Void moveAssign(TreeBaseType&& tree, std::true_type)
    {
        clear();
        rootNode() = tree.move(tree.rootNode());
        nodeAlloc() = std::move(tree.nodeAlloc());
    }

    Void moveAssign(TreeBaseType&& tree, std::false_type)
    {
        clear();
        if (nodeAlloc() == tree.nodeAlloc()) {
            rootNode() = tree.move(tree.rootNode());
        } else {
            rootNode() = copy(tree.root());
            tree.clear();
        }
    }

    Void clear()
    {
        destroy(rootNode());
    }

private:
    NodeAlloc   mNodeAl;
    NodePtr     mRoot;

};

}

template <class Val, class Alloc = std::allocator<Val>>
class Tree
    : public TreeBase<Val, Alloc>
{
    using TreeType              = Tree<Val, Alloc>;
    using Base                  = TreeBase<Val, Alloc>;
    using AllocTraits           = typename Base::AllocTraits;
    using Node                  = typename Base::Node;
    using NodeAlloc             = typename Base::NodeAlloc;
    using NodeAllocTraits       = typename Base::NodeAllocTraits;
    using NodePtr               = typename Base::NodePtr;

    using PreIter               = typename Base::PreIter;
    using PostIter              = typename Base::PostIter;
    using ChildIter             = typename Base::ChildIter;

    using ConstPreIter          = typename Base::ConstPreIter;
    using ConstPostIter         = typename Base::ConstPostIter;
    using ConstChildIter        = typename Base::ConstChildIter;

public:
    using AllocatorType         = typename Base::AllocatorType;
    using ValueType             = typename Base::ValueType;
    using Reference             = typename Base::Reference;
    using Pointer               = typename Base::Pointer;
    using ConstPointer          = typename Base::ConstPointer;
    using ConstReference        = typename Base::ConstReference;
    using SizeType              = typename Base::SizeType;
    using DifferenceType        = typename Base::DifferenceType;

    using allocator_type        = AllocatorType;
    using value_type            = ValueType;
    using reference             = Reference;
    using const_reference       = ConstReference;
    using pointer               = Pointer;
    using const_pointer         = ConstPointer;
    using size_type             = SizeType;
    using difference_type       = DifferenceType;

    using Crawler               = typename Base::Crawler;
    using ConstCrawler          = typename Base::ConstCrawler;

    Tree()
        : Base()
    {
    }

    explicit Tree(const AllocatorType& alloc)
        : Base(alloc)
    {
    }

    explicit Tree(ConstReference val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(val);
    }

    explicit Tree(ValueType&& val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(std::move(val));
    }

    template <class Crawler2, typename = std::enable_if_t<
        !std::is_base_of<TreeType, std::decay_t<Crawler2>>::value &&
            !std::is_convertible<ValueType, std::decay_t<Crawler2>>::value
        >
    >
    explicit Tree(Crawler2 cw,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::copy(cw);
    }

    Tree(const TreeType& tree)
        : Base(tree)
    {
    }

    Tree(TreeType&& tree)
        : Base(std::move(tree))
    {
    }

    ~Tree()
    {
    }

    TreeType& operator=(const TreeType& tree)
    {
        Base::operator=(tree);
        return *this;
    }

    TreeType& operator=(TreeType&& tree)
    {
        Base::operator=(std::move(tree));
        return *this;
    }

    Bool empty() const
    {
        return !Base::rootNode();
    }

    SizeType size() const
    {
        return std::distance(PreIter::begin(root()),
            PreIter::end(root()));
    }

    SizeType maxSize() const
    {
        return NodeAllocTraits::max_size(Base::nodeAlloc());
    }

    SizeType max_size() const
    {
        return maxSize();
    }

    Crawler root()
    {
        return Base::root();
    }

    ConstCrawler root() const
    {
        return Base::root();
    }

    ConstCrawler croot() const
    {
        return root();
    }

    AllocatorType getAllocator() const
    {
        return Base::getAllocator();
    }

    AllocatorType get_allocator() const
    {
        return getAllocator();
    }

    Void reset(ConstReference val)
    {
        clear();
        Base::rootNode() = Base::createNode(val);
    }

    Void reset(ValueType&& val)
    {
        clear();
        Base::rootNode() = Base::createNode(std::move(val));
    }

    template <class Crawler2>
    Void assign(Crawler2 rootcw)
    {
        clear();
        Base::rootNode() = Base::copy(rootcw);
    }

    template <class Crawler2>
    Void assign(Crawler2 rootcw, Crawler at)
    {
        if (!parent(at)) {
            assign(rootcw);
            return;
        }

        auto p = parent(at);
        auto r = right(at);

        Base::destory(at.node());
        p.node()->insert(Base::copy(rootcw), r.node());
    }

    Void clear()
    {
        Base::clear();
    }

    Void pushBack(ConstCrawler parentcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        pushBackImpl(parentcw, node);
    }

    Void pushBack(ConstCrawler parentcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        pushBackImpl(parentcw, node);
    }

    template <class... Args>
    Void emplaceBack(ConstCrawler parentcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        pushBackImpl(parentcw, node);
    }

    Void pushFront(ConstCrawler parentcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        pushFrontImpl(parentcw, node);
    }

    Void pushFront(ConstCrawler parentcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        pushFrontImpl(parentcw, node);
    }

    template <class... Args>
    Void emplaceFront(ConstCrawler parentcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        pushFrontImpl(parentcw, node);
    }

    Void insert(ConstCrawler rightcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        insertImpl(rightcw, node);
    }

    Void insert(ConstCrawler rightcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        insertImpl(rightcw, node);
    }

    template <class... Args>
    Void emplace(ConstCrawler rightcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        insertImpl(rightcw, node);
    }

    template <class Crawler2, typename = std::enable_if_t<
        !std::is_convertible<ValueType, std::decay_t<Crawler2>>::value
        >
    >
    Void insert(ConstCrawler rightcw, Crawler2 rootcw)
    {
        auto node = Base::copy(rootcw);
        insertImpl(rightcw, node);
    }

    TreeType popBack(ConstCrawler parentcw)
    {
        return TreeType(Base::nodeAlloc(),
            popBackImpl(parentcw));
    }

    TreeType popFront(ConstCrawler parentcw)
    {
        return TreeType(Base::nodeAlloc(),
            popFrontImpl(parentcw));
    }

    TreeType remove(ConstCrawler cw)
    {
        return TreeType(Base::nodeAlloc(),
            removeImpl(cw));
    }

    Void erase(ConstCrawler cw)
    {
        Base::destroy(cw.node());
    }

    Void splice(ConstCrawler rightcw, TreeType& tree)
    {
        insertImpl(rightcw, tree.rootNode());
        tree.clearRoot();
    }

    Void splice(ConstCrawler rightcw, TreeType&& tree)
    {
        insertImpl(rightcw, tree.removeImpl(tree.root()));
    }

    Void splice(ConstCrawler rightcw, TreeType& tree,
        ConstCrawler rootcw)
    {
        insertImpl(rightcw, tree.removeImpl(rootcw));
    }

    Void splice(ConstCrawler rightcw, TreeType&& tree,
        ConstCrawler rootcw)
    {
        insertImpl(rightcw, tree.removeImpl(rootcw));
    }

private:
    Tree(const NodeAlloc& alloc, NodePtr node)
        : Base(alloc, node)
    {
    }

    Void pushBackImpl(ConstCrawler parentcw, NodePtr node)
    {
        parentcw.node()->pushBack(node);
    }

    Void pushFrontImpl(ConstCrawler parentcw, NodePtr node)
    {
        parentcw.node()->pushFront(node);
    }

    Void insertImpl(ConstCrawler rightcw, NodePtr node)
    {
        parent(rightcw).node()->insert(node, rightcw.node());
    }

    NodePtr popBackImpl(ConstCrawler parentcw)
    {
        return parentcw.node()->popBack();
    }

    NodePtr popFrontImpl(ConstCrawler parentcw)
    {
        return parentcw.node()->popFront();
    }

    NodePtr removeImpl(ConstCrawler cw)
    {
        if (parent(cw)) {
            return parent(cw).node()->remove(cw.node());
        } else {
            Base::clearRoot();
            return cw.node();
        }
    }

};

template <class Val, class Alloc>
Bool operator==(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    using PreIter = PreIterator<typename Tree<Val, Alloc>::ConstCrawler>;

    return std::equal(
        PreIter::begin(l.root()),
        PreIter::end(l.root()),
        PreIter::begin(r.root()),
        PreIter::end(r.root())
    );
}

template <class Val, class Alloc>
Bool operator!=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l == r);
}

template <class Val, class Alloc>
Bool operator<(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    using PreIter = PreIterator<typename Tree<Val, Alloc>::ConstCrawler>;

    return std::lexicographical_compare(
        PreIter::begin(l.root()),
        PreIter::end(l.root()),
        PreIter::begin(r.root()),
        PreIter::end(r.root())
    );
}

template <class Val, class Alloc>
Bool operator>(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return r < l;
}

template <class Val, class Alloc>
Bool operator<=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l > r);
}

template <class Val, class Alloc>
Bool operator>=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l < r);
}

}
}

#endif