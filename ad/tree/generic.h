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

/**
 * @file    generic.h
 * @brief   Generic tree class definition
 *
 * This file defines the generic tree class. A generic tree is a
 * rooted tree with any number of nodes and any number of child
 * nodes per parent node. It provides methods to construct, grow
 * , shrink and traverse the tree structure via crawlers.
 */

#ifndef AD_TREE_GENERIC_H_
#define AD_TREE_GENERIC_H_

#include <memory>
#include <type_traits>
#include <iterator>
#include <algorithm>
#include "ad/types.h"
#include "ad/tree/iterator.h"

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

        copyAssign(tree, std::integral_constant<Bool, NodeAllocTraits::
            propagate_on_container_copy_assignment::value>());

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
        copyAssignAlloc(tree.nodeAlloc(),
            std::integral_constant<Bool, truth>());
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

/**
 * @brief   Generic tree class
 *
 * @tparam  Val     The type of the items that will be stored in the tree
 * @tparam  Alloc   The allocator type that will be used by the tree to
 *                  allocate and construct tree nodes
 *
 *
 * A generic tree is a rooted tree with any number of nodes and any
 * number of child nodes per parent node. It provides methods to
 * construct, grow, shrink and traverse the tree structure via crawlers.
 * Crawlers are to trees as iterators are to STL containers. They
 * provide the bridge between the tree data structures and the
 * algorithms. To learn more about crawlers please check its documentation.
 */
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

    /**
     * @brief   Default constructor
     *
     * Constructs an empty tree
     *
     * Complexity   : Constant
     */
    Tree()
        : Base()
    {
    }

    /**
     * @brief   Construct with specified allocator
     *
     * @param   alloc   The allocator object to construct from
     *
     * Constructs an empty tree but with given allocator
     *
     * Complexity   : Same as <Alloc>'s copy constructor
     */
    explicit Tree(const AllocatorType& alloc)
        : Base(alloc)
    {
    }

    /**
     * @brief   Construct with root and allocator
     *
     * @param   val     The value of the root of the tree
     * @param   alloc   The allocator object to construct from
     *
     * Constructs a tree with one node - the root which is initialized
     * with a copy of the given value <val> and uses the specified
     * allocator <alloc>
     *
     * Complexity   : <Val>'s copy constructor + <Alloc>'s copy constructor
     */
    explicit Tree(ConstReference val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(val);
    }

    /**
     * @brief   Construct with root and allocator
     *
     * @param   val     The value of the root of the tree
     * @param   alloc   The allocator object to construct from
     *
     * Constructs a tree with one node - the root which is initialized
     * by moving the value <val> into the node and uses the specified
     * allocator <alloc>
     *
     * Complexity   : <Val>'s move constructor + <Alloc>'s copy constructor
     */
    explicit Tree(ValueType&& val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(std::move(val));
    }

    /**
     * @brief   Construct from another tree
     *
     * @tparam  Crawler2    A crawler type
     *
     * @param   cw      A crawler to the root of some subtree
     * @param   alloc   The allocator object to construct from
     *
     * Constructs a tree by copying the subtree rooted at <cw>
     * using the specified allocator <alloc>. This constructor
     * is used only when <Crawler2> is not a generic tree or
     * a type which is convertible to the value type <Val> of
     * the tree.
     *
     * Complexity   : If <Sub> is the subtree rooted at <cw> then
     *                the complexity is - Pre-order traversal complexity
     *                of <Sub> + Pre-order complexity of <this> +
     *                size of <Sub> invocations of Val(*cw) +
     *                <Alloc>'s copy constructor
     */
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

    /**
     * @brief   Copy constructor
     *
     * @param   tree    The tree to copy from
     *
     * Complexity   : Pre-order traversal of <tree> + size of <tree>
     *                invocations of the copy constructor + <Alloc>'s
     *                copy constructor
     */
    Tree(const TreeType& tree)
        : Base(tree)
    {
    }

    /**
     * @brief   Move constructor
     *
     * @param   tree    The tree to move from
     *
     * After this operation the <tree> will be in an unspecified state
     * and should not be used.
     *
     * Complexity   : <Alloc>'s move constructor
     */
    Tree(TreeType&& tree)
        : Base(std::move(tree))
    {
    }

    /**
     * @brief   Destructor
     *
     * Destroys the tree and frees memory
     *
     * Complexity   : <this>'s post order traversal +
     *                destructor calls for each item
     */
    ~Tree()
    {
    }

    /**
     * @brief   Copy assigment operator
     *
     * @param   tree    The tree to copy from
     *
     * @return  A reference to itself
     *
     * Complexity   : Post-oder traversal of <this> + size of <this>
     *                destructor calls of <Val> + Pre-oder traversal of
     *                <tree> + size of <tree> invocations of <Val>'s copy
     *                constructors + <Alloc>'s assignment operator if
     *                <Alloc>::propagate_on_container_copy_assignment is true
     */
    TreeType& operator=(const TreeType& tree)
    {
        Base::operator=(tree);
        return *this;
    }

    /**
     * @brief   Move assignment operator
     *
     * @param   tree    The tree to move from
     *
     * @return  A reference to itself
     *
     * After this operation the <tree> will be in an unspecified state
     * and should not be used.
     *
     * Complexity   : Post-order traversal of <this> + size of <this>
     *                destructor calls of <Val> + Constant if <Alloc>::
     *                propagate_on_container_move_assignment is true
     *                or if it is false and allocators compare equal else
     *                pre-order traversal of <tree> and <this> and
     *                size of <tree> <Val>'s move constructor calls
     */
    TreeType& operator=(TreeType&& tree)
    {
        Base::operator=(std::move(tree));
        return *this;
    }

    /**
     * @brief   Check if the tree is empty
     *
     * @return  True if the tree is empty false otherwise
     *
     * Complexity   : Constant
     */
    Bool empty() const
    {
        return !Base::rootNode();
    }

    /**
     * @brief   Get the size of the tree
     *
     * @return  The size of the tree
     *
     * size() for this tree structure is NOT A CONSTANT time
     * operation. This is so that operations to grow and prune a
     * tree are efficient
     *
     * Complexity   : Pre-order traversal of <this>
     */
    SizeType size() const
    {
        return std::distance(PreIter::begin(root()),
            PreIter::end(root()));
    }

    /**
     * @brief   Get the maximum size of the tree
     *
     * @return  The maximum size of the tree
     *
     * Although this tree structure is defined to support any number
     * of nodes, in reality it is limited by the available memory
     *
     * Complexity   : <Alloc traits>::max_size() complexity
     */
    SizeType maxSize() const
    {
        return NodeAllocTraits::max_size(Base::nodeAlloc());
    }

    /**
     * @brief   Get the maximum size of the tree
     *
     * @return  The maximum size of the tree
     *
     * Although this tree structure is defined to support any number
     * of nodes, in reality it is limited by the available memory. This
     * method is provided to maintain compatibility with STL naming
     * conventions
     *
     * Complexity   : <Alloc traits>::max_size() complexity
     */
    SizeType max_size() const
    {
        return maxSize();
    }

    /**
     * @brief   Get a crawler to the root of the tree
     *
     * @return  A crawler to the root of the tree
     *
     * Complexity   : Constant
     */
    Crawler root()
    {
        return Base::root();
    }

    /**
     * @brief   Get a const crawler to the root of the tree
     *
     * @return  A const crawler to the root of the tree
     *
     * Complexity   : Constant
     */
    ConstCrawler root() const
    {
        return Base::root();
    }

    /**
     * @brief   Get a const crawler to the root of the tree
     *
     * @return  A const crawler to the root of the tree
     *
     * Complexity   : Constant
     */
    ConstCrawler croot() const
    {
        return root();
    }

    /**
     * @brief   Get the allocator used by the tree
     *
     * @return  The allocator that is used by the tree
     *
     * Complexity   : Constant
     */
    AllocatorType getAllocator() const
    {
        return Base::getAllocator();
    }

    /**
     * @brief   Get the allocator used by the tree
     *
     * @return  The allocator that is used by the tree
     *
     * This method is provided to maintain compatibility with the STL
     * naming conventions
     *
     * Complexity   : Constant
     */
    AllocatorType get_allocator() const
    {
        return getAllocator();
    }

    /**
     * @brief   Reset the tree
     *
     * @param   val     The value to assign to the root
     *
     * This method destroys the tree and then reinitializes it as
     * a tree with a single root node with value <val>
     *
     * Complexity   : Post-order traversal of <this> +
     *                <Val>'s copy constructor
     */
    Void reset(ConstReference val)
    {
        clear();
        Base::rootNode() = Base::createNode(val);
    }

    /**
     * @brief   Reset the tree
     *
     * @param   val     The value to assign to the root
     *
     * This method destroys the tree and then reinitializes it as
     * a tree with a single root node with value <val>
     *
     * Complexity   : Post-order traversal of <this> +
     *                <Val>'s move constructor
     */
    Void reset(ValueType&& val)
    {
        clear();
        Base::rootNode() = Base::createNode(std::move(val));
    }

    /**
     * @brief   Replace this tree with another
     *
     * @param   rootcw      A crawler to the root of the subtree with
     *                      which to replace <this>'s contents
     *
     * This method replaces the tree to a copy of the subtree rooted
     * at <rootcw>
     *
     * Complexity   : Post-order traversal of <this> + size of <this>
     *                invocations of <Val>'s destructor + Pre-order
     *                traversal of <tree> and the new <this> + size
     *                of <tree> invocations of <Val>'s copy constructor
     */
    template <class Crawler2>
    Void assign(Crawler2 rootcw)
    {
        clear();
        Base::rootNode() = Base::copy(rootcw);
    }

    /**
     * @brief   Replace a subtree with another
     *
     * @param   rootcw      A crawler to the root of the subtree with
     *                      which to replace <this>'s subtree's contents
     * @param   at          The subtree to replace
     *
     * This method replaces the subtree rooted at <at> to a copy of the
     * subtree rooted at <rootcw>
     *
     * Complexity   : If <sub> is the subtree rooted at <at> then -
     *                Post-order traversal of <sub> + size of <sub>
     *                invocations of <Val>'s destructor + Pre-order
     *                traversal of <tree> and the new <sub> + size
     *                of <tree> invocations of <Val>'s copy constructor
     */
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

    /**
     * @brief   Destory the tree
     *
     * Destroys the entire tree and its contents i.e the tree
     * will be empty after this operation
     *
     * Complexity   : Post-order traversal of <this> + size of <this>
     *                invocations of <Val>'s destructor
     */
    Void clear()
    {
        Base::clear();
    }

    /**
     * @brief   Add a last child to a parent
     *
     * @param   parentcw    A crawler to the parent node
     * @param   val         The value the last child should hold
     *
     * Adds a last child to the parent node then copies <val> into that node
     *
     * Complexity   : <Val>'s copy constructor
     */
    Void pushBack(ConstCrawler parentcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        pushBackImpl(parentcw, node);
    }

    /**
     * @brief   Add a last child to a parent
     *
     * @param   parentcw    A crawler to the parent node
     * @param   val         The value the last child should hold
     *
     * Adds a last child to the parent node then moves <val> into that node
     *
     * Complexity   : <Val>'s move constructor
     */
    Void pushBack(ConstCrawler parentcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        pushBackImpl(parentcw, node);
    }

    /**
     * @brief   Add a last child to a parent
     *
     * @tparam  Args        List of the types of the arguments to
     *                      construct <Val>
     *
     * @param   parentcw    A crawler to the parent node
     * @param   args        The arguments to construct <Val> in place
     *
     * Adds a last child to the parent node then constructs <Val>
     * in place and stores it in that node
     *
     * Complexity   : Val(args) constructor complexity
     */
    template <class... Args>
    Void emplaceBack(ConstCrawler parentcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        pushBackImpl(parentcw, node);
    }

    /**
     * @brief   Add a first child to a parent
     *
     * @param   parentcw    A crawler to the parent node
     * @param   val         The value the first child should hold
     *
     * Adds a first child to the parent node then copies <val> into that node
     *
     * Complexity   : <Val>'s copy constructor
     */
    Void pushFront(ConstCrawler parentcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        pushFrontImpl(parentcw, node);
    }

    /**
     * @brief   Add a first child to a parent
     *
     * @param   parentcw    A crawler to the parent node
     * @param   val         The value the first child should hold
     *
     * Adds a first child to the parent node then moves <val> into that node
     *
     * Complexity   : <Val>'s move constructor
     */
    Void pushFront(ConstCrawler parentcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        pushFrontImpl(parentcw, node);
    }

    /**
     * @brief   Add a first child to a parent
     *
     * @tparam  Args        List of the types of the arguments to
     *                      construct <Val>
     *
     * @param   parentcw    A crawler to the parent node
     * @param   args        The arguments to construct <Val> in place
     *
     * Adds a first child to the parent node then constructs <Val>
     * in place and stores it in that node
     *
     * Complexity   : Val(args) constructor complexity
     */
    template <class... Args>
    Void emplaceFront(ConstCrawler parentcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        pushFrontImpl(parentcw, node);
    }

    /**
     * @brief   Add a child to a parent
     *
     * @param   rightcw     A crawler to the right child before which
     *                      this <val> should be added
     * @param   val         The value the child should hold
     *
     * Inserts a child to the parent node before <rightcw> then
     * copies <val> into that node. Note that <rightcw> cannot
     * be the root of the tree. The behaviour is undefined if it is.
     *
     * Complexity   : <Val>'s copy constructor
     */
    Void insert(ConstCrawler rightcw, ConstReference val)
    {
        auto node = Base::createNode(val);
        insertImpl(rightcw, node);
    }

    /**
     * @brief   Add a child to a parent
     *
     * @param   rightcw     A crawler to the right child before which
     *                      this <val> should be added
     * @param   val         The value the child should hold
     *
     * Inserts a child to the parent node before <rightcw> then
     * moves <val> into that node. Note that <rightcw> cannot be
     * the root of the tree. The behaviour is undefined if it is.
     *
     * Complexity   : <Val>'s move constructor
     */
    Void insert(ConstCrawler rightcw, ValueType&& val)
    {
        auto node = Base::createNode(std::move(val));
        insertImpl(rightcw, node);
    }

    /**
     * @brief   Add a child to a parent
     *
     * @tparam  Args        List of the types of the arguments to
     *                      construct <Val>
     *
     * @param   rightcw     A crawler to the right child before which
     *                      this <val> should be added
     * @param   args        The arguments to construct <Val> in place
     *
     * Inserts a child to the parent node before <rightcw> then
     * constructs <Val> in place and stores it into that node
     * Note that <rightcw> cannot be the root of the tree. The
     * behaviour is undefined if it is.
     *
     * Complexity   : Val(args) constructor complexity
     */
    template <class... Args>
    Void emplace(ConstCrawler rightcw, Args&&... args)
    {
        auto node = Base::createNode(std::forward<Args>(args)...);
        insertImpl(rightcw, node);
    }

    /**
     * @brief   Inserts a copy of a subtree into the tree
     *
     * @tparam  Crawler2    Type of the crawler of the subtree to be copied
     *
     * @param   rightcw     A crawler to the right child before which
     *                      the subtree rooted at <rootcw> should be inserted
     * @param   rootcw      A crawler to the subtree that should be copied
     *
     * Makes a copy of the subtee rooted at <rootcw> and inserts it before
     * <rightcw>. Note that <rightcw> cannot be the root of the tree. The
     * behaviour is undefined if it is.
     *
     * Complexity   : If <sub> is the subtree rooted at <rootcw> then -
     *                pre-order traversal of <sub> + size of <sub>
     *                invocations of <Val>'s copy constructor
     */
    template <class Crawler2, typename = std::enable_if_t<
        !std::is_convertible<ValueType, std::decay_t<Crawler2>>::value
        >
    >
    Void insert(ConstCrawler rightcw, Crawler2 rootcw)
    {
        auto node = Base::copy(rootcw);
        insertImpl(rightcw, node);
    }

    /**
     * @brief   Unlinks the last subtree of a parent node
     *
     * @param   parentcw    A crawler to the parent node
     *
     * @return  The last subtree of the parent
     *
     * The last subtree of the parent is unlinked and returned as
     * another tree object.
     *
     * Complexity   : Constant
     */
    TreeType popBack(ConstCrawler parentcw)
    {
        return TreeType(Base::nodeAlloc(),
            popBackImpl(parentcw));
    }

    /**
     * @brief   Unlinks the first subtree of a parent node
     *
     * @param   parentcw    A crawler to the parent node
     *
     * @return  The first subtree of the parent
     *
     * The first subtree of the parent is unlinked and returned as
     * another tree object.
     *
     * Complexity   : Constant
     */
    TreeType popFront(ConstCrawler parentcw)
    {
        return TreeType(Base::nodeAlloc(),
            popFrontImpl(parentcw));
    }

    /**
     * @brief   Unlinks a subtree
     *
     * @param   cw    A crawler to the root of the subtree
     *
     * @return  The subtree that was removed (unlinked)
     *
     * Note that if <cw> is the root of the tree than the
     * tree will be empty after this operation
     *
     * Complexity   : Constant
     */
    TreeType remove(ConstCrawler cw)
    {
        return TreeType(Base::nodeAlloc(),
            removeImpl(cw));
    }

    /**
     * @brief   Destroys a subtree
     *
     * @param   cw    A crawler to the root of the subtree
     *
     * The subtree rooted at <cw> will be destroyed after this operation.
     * Note that if <cw> is the root of the tree then the entire tree
     * will be destroyed.
     *
     * Complexity   : If <sub> is the subtree rooted at <cw> then -
     *                post-order traversal of <sub> + size of <sub>
     *                invocations of <Val>'s destructor
     */
    Void erase(ConstCrawler cw)
    {
        Base::destroy(cw.node());
    }

    /**
     * @brief   Splices a tree into this tree
     *
     * @param   rightcw     A crawler to the right child before which this
     *                      tree needs to be inserted
     * @param   tree        The tree to be inserted
     *
     * Insert the tree by modifying its root node links before the child
     * <rightcw>. Since this operation only changes links it is required
     * that <this> can take ownership of <tree>'s memory. This implies
     * that their allocators should compare equal, however currently
     * this assumption is not asserted in this method. After this operation
     * <tree> will be empty.
     *
     * Complexity   : Constant
     */
    Void splice(ConstCrawler rightcw, TreeType& tree)
    {
        insertImpl(rightcw, tree.rootNode());
        tree.clearRoot();
    }

    /**
     * @brief   Splices a tree into this tree
     *
     * @param   rightcw     A crawler to the right child before which this
     *                      tree needs to be inserted
     * @param   tree        The tree to be inserted
     *
     * Insert the tree by modifying its root node links before the child
     * <rightcw>. Since this operation only changes links it is required
     * that <this> can take ownership of <tree>'s memory. This implies
     * that their allocators should compare equal, however currently
     * this assumption is not asserted in this method. After this operation
     * <tree> will be empty.
     *
     * Complexity   : Constant
     */
    Void splice(ConstCrawler rightcw, TreeType&& tree)
    {
        insertImpl(rightcw, tree.removeImpl(tree.root()));
    }

    /**
     * @brief   Splices a subtree into this tree
     *
     * @param   rightcw     A crawler to the right child before which this
     *                      subtree needs to be inserted
     * @param   tree        The subtree to be inserted
     * @param   rootcw      A crawler to the root of <tree>'s subtree
     *
     * Insert the subtree of <tree> rooted at <rootcw> by modifying its
     * root node links before the child <rightcw>. Since this operation only
     * changes links it is required that <this> can take ownership of
     * <tree>'s memory. This implies that their allocators should compare
     * equal, however currently this assumption is not asserted in this method.
     * After this operation the subtree at <rootcw> will be removed for <tree>.
     *
     * Complexity   : Constant
     */
    Void splice(ConstCrawler rightcw, TreeType& tree,
        ConstCrawler rootcw)
    {
        insertImpl(rightcw, tree.removeImpl(rootcw));
    }

    /**
     * @brief   Splices a subtree into this tree
     *
     * @param   rightcw     A crawler to the right child before which this
     *                      subtree needs to be inserted
     * @param   tree        The subtree to be inserted
     * @param   rootcw      A crawler to the root of <tree>'s subtree
     *
     * Insert the subtree of <tree> rooted at <rootcw> by modifying its
     * root node links before the child <rightcw>. Since this operation only
     * changes links it is required that <this> can take ownership of
     * <tree>'s memory. This implies that their allocators should compare
     * equal, however currently this assumption is not asserted in this method.
     * After this operation the subtree at <rootcw> will be removed for <tree>.
     *
     * Complexity   : Constant
     */
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

/**
 * @brief   Operator == overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : Pre-order traversal of l and r + size of <l>
 *                invocations of <Val>'s == operator if size
 *                of <l> == size of <r>, constant otherwise
 *
 */
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

/**
 * @brief   Operator != overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : Pre-order traversal of l and r + size of <l>
 *                invocations of <Val>'s == operator if size
 *                of <l> == size of <r>, constant otherwise
 *
 */
template <class Val, class Alloc>
Bool operator!=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l == r);
}

/**
 * @brief   Operator < overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : 2 * min(pre-order traversal of <l> and <r>) +
 *                2 * min(sizeof <l>, <r> invocations of <Val>'s < operator)
 *
 */
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

/**
 * @brief   Operator > overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : 2 * min(pre-order traversal of <l> and <r>) +
 *                2 * min(sizeof <l>, <r> invocations of <Val>'s < operator)
 *
 */
template <class Val, class Alloc>
Bool operator>(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return r < l;
}

/**
 * @brief   Operator <= overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : 2 * min(pre-order traversal of <l> and <r>) +
 *                2 * min(sizeof <l>, <r> invocations of <Val>'s < operator)
 *
 */
template <class Val, class Alloc>
Bool operator<=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l > r);
}

/**
 * @brief   Operator >= overload for trees
 *
 * @tparam  Val     Value type of the trees
 * @tparam  Alloc   Allocator type of the trees
 *
 * @param   l       The LHS tree
 * @param   r       The RHS tree
 *
 * Complexity   : 2 * min(pre-order traversal of <l> and <r>) +
 *                2 * min(sizeof <l>, <r> invocations of <Val>'s < operator)
 *
 */
template <class Val, class Alloc>
Bool operator>=(const Tree<Val, Alloc>& l, const Tree<Val, Alloc>& r)
{
    return !(l < r);
}

}
}

#endif