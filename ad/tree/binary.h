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

#ifndef AD_TREE_BINARY_H_
#define AD_TREE_BINARY_H_

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
namespace det
{

template <class Val, class VoidPtr>
class BinaryTreeNode
{
    using NodeType          = BinaryTreeNode<Val, VoidPtr>;

public:
    using ValueType         = Val;
    using Reference         = Val&;
    using RawPtr            = Val*;
    using NodePtr           = typename std::pointer_traits<VoidPtr>::
        template rebind<NodeType>;

    RawPtr rawPtr()
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

    NodePtr leftChild()
    {
        return mLeftChild;
    }

    NodePtr rightChild()
    {
        return mRightChild;
    }

    Void clear()
    {
        mParent = mLeftChild = mRightChild = nullptr;
    }

    Void insertLeft(NodePtr child)
    {
        mLeftChild = child;
        child->mParent = this;
    }

    Void insertRight(NodePtr child)
    {
        mRightChild = child;
        child->mParent = this;
    }

    NodePtr remove(NodePtr child)
    {
        if (child == mLeftChild) {
            mLeftChild = nullptr;
        } else {
            mRightChild = nullptr;
        }
        child->mParent = nullptr;
        return child;
    }

private:
    NodePtr     mParent;
    NodePtr     mLeftChild;
    NodePtr     mRightChild;

    ValueType   mVal;

}; /* class BinaryTreeNode */

template <class, class> class BinaryTreeBase;

} /* namespace det */

template <class Val, class VoidPtr>
class BinaryTreeVisitor
{
    using VisitorType           = BinaryTreeVisitor<Val, VoidPtr>;
    using NodePtr               = typename det::BinaryTreeNode<Val, VoidPtr>::NodePtr;

    template <class, class> friend class det::BinaryTreeBase;
    template <class, class> friend class BinaryTree;
    template <class, class> friend class BinaryTreeConstVisitor;

public:
    using ValueType             = Val;
    using Reference             = Val&;
    using Pointer               = typename std::pointer_traits<VoidPtr>::
        template rebind<ValueType>;
    using DifferenceType        = typename std::pointer_traits<Pointer>::
        difference_type;

    BinaryTreeVisitor()
        : mNode(nullptr)
    {
    }

    BinaryTreeVisitor(const VisitorType& visitor)
        : mNode(visitor.node())
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

    VisitorType parent() const
    {
        return VisitorType(mNode->parent());
    }

    VisitorType first() const
    {
        return VisitorType(mNode->leftChild() ? mNode->leftChild() :
            mNode->rightChild());
    }

    VisitorType last() const
    {
        return VisitorType(mNode->rightChild() ? mNode->rightChild() :
            mNode->leftChild());
    }

    VisitorType left() const
    {
        if (!mNode->parent() || mNode->parent()->leftChild() == mNode) {
            return VisitorType();
        } else {
            return VisitorType(mNode->parent()->leftChild());
        }
    }

    VisitorType right() const
    {
        if (!mNode->parent() || mNode->parent()->rightChild() == mNode) {
            return VisitorType();
        } else {
            return VisitorType(mNode->parent()->rightChild());
        }
    }

    VisitorType leftChild() const
    {
        return VisitorType(mNode->leftChild());
    }

    VisitorType rightChild() const
    {
        return VisitorType(mNode->rightChild());
    }

    explicit operator Bool() const
    {
        return mNode != nullptr;
    }

    inline friend Bool operator==(const VisitorType& l,
        const VisitorType& r)
    {
        return l.node() == r.node();
    }

    inline friend Bool operator!=(const VisitorType& l,
        const VisitorType& r)
    {
        return !(l == r);
    }

private:
    explicit BinaryTreeVisitor(NodePtr ptr)
        : mNode(ptr)
    {
    }

    NodePtr node() const
    {
        return mNode;
    }

    NodePtr     mNode;

}; /* class BinaryTreeVisitor */

template <class Val, class VoidPtr>
class BinaryTreeConstVisitor
{
    using ConstVisitorType      = BinaryTreeConstVisitor<Val, VoidPtr>;
    using VisitorType           = BinaryTreeVisitor<Val, VoidPtr>;
    using NodePtr               = typename det::BinaryTreeNode<Val, VoidPtr>::NodePtr;

    template <class, class> friend class det::BinaryTreeBase;
    template <class, class> friend class BinaryTree;

public:
    using ValueType             = Val;
    using Reference             = const Val&;
    using Pointer               = typename std::pointer_traits<VoidPtr>::
        template rebind<const ValueType>;
    using DifferenceType        = typename std::pointer_traits<Pointer>::
        difference_type;

    BinaryTreeConstVisitor()
        : mNode(nullptr)
    {
    }

    BinaryTreeConstVisitor(const VisitorType& visitor)
        : mNode(visitor.node())
    {
    }

    BinaryTreeConstVisitor(const ConstVisitorType& visitor)
        : mNode(visitor.node())
    {
    }

    ConstVisitorType& operator=(const VisitorType& visitor)
    {
        mNode = visitor.node();
        return *this;
    }

    ConstVisitorType& operator=(const ConstVisitorType& visitor)
    {
        mNode = visitor.node();
        return *this;
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

    ConstVisitorType parent() const
    {
        return ConstVisitorType(mNode->parent());
    }

    ConstVisitorType first() const
    {
        return ConstVisitorType(mNode->leftChild() ? mNode->leftChild() :
            mNode->rightChild());
    }

    ConstVisitorType last() const
    {
        return ConstVisitorType(mNode->rightChild() ? mNode->rightChild() :
            mNode->leftChild());
    }

    ConstVisitorType left() const
    {
        if (!mNode->parent() || mNode->parent()->leftChild() == mNode) {
            return ConstVisitorType();
        } else {
            return ConstVisitorType(mNode->parent()->leftChild());
        }
    }

    ConstVisitorType right() const
    {
        if (!mNode->parent() || mNode->parent()->rightChild() == mNode) {
            return ConstVisitorType();
        } else {
            return ConstVisitorType(mNode->parent()->rightChild());
        }
    }

    ConstVisitorType leftChild() const
    {
        return ConstVisitorType(mNode->leftChild());
    }

    ConstVisitorType rightChild() const
    {
        return ConstVisitorType(mNode->rightChild());
    }

    explicit operator Bool() const
    {
        return mNode != nullptr;
    }

    inline friend Bool operator==(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return l.node() == r.node();
    }

    inline friend Bool operator!=(const ConstVisitorType& l,
        const ConstVisitorType& r)
    {
        return !(l == r);
    }

private:
    explicit BinaryTreeConstVisitor(NodePtr ptr)
        : mNode(ptr)
    {
    }

    NodePtr node() const
    {
        return mNode;
    }

    NodePtr     mNode;

}; /* class BinaryTreeConstVisitor */

namespace det
{

template <class Val, class Alloc>
class BinaryTreeBase
{
    using TreeBaseType          = BinaryTreeBase<Val, Alloc>;

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
    using Node                  = det::BinaryTreeNode<Val, VoidPtr>;
    using NodeAlloc             = typename AllocTraits::template rebind_alloc<Node>;
    using NodeAllocTraits       = std::allocator_traits<NodeAlloc>;
    using NodePtr               = typename Node::NodePtr;

    using Visitor               = BinaryTreeVisitor<Val, VoidPtr>;
    using ConstVisitor          = BinaryTreeConstVisitor<Val, VoidPtr>;

    using PreIter               = PreIterator<Visitor>;
    using PostIter              = PostIterator<Visitor>;
    using ChildIter             = ChildIterator<Visitor>;

    using ConstPreIter          = PreIterator<ConstVisitor>;
    using ConstPostIter         = PostIterator<ConstVisitor>;
    using ConstChildIter        = ChildIterator<ConstVisitor>;

    BinaryTreeBase()
        : mNodeAl(), mRoot(nullptr)
    {
    }

    explicit BinaryTreeBase(const AllocatorType& alloc)
        : mNodeAl(alloc), mRoot(nullptr)
    {
    }

    BinaryTreeBase(const NodeAlloc& nodeAl, NodePtr root)
        : mNodeAl(nodeAl), mRoot(root)
    {
    }

    BinaryTreeBase(const BinaryTreeBase& tree)
        : mNodeAl(NodeAllocTraits::
            select_on_container_copy_construction(tree.nodeAlloc())),
            mRoot(copy(tree.root()))
    {
    }

    BinaryTreeBase(BinaryTreeBase&& tree)
        : mNodeAl(std::move(tree.nodeAlloc())),
            mRoot(tree.cut(tree.rootNode()))
    {
    }

    ~BinaryTreeBase()
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

        moveAssign(std::move(tree), typename NodeAllocTraits::
            propagate_on_container_move_assignment());

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

    Visitor root()
    {
        return Visitor(rootNode());
    }

    ConstVisitor root() const
    {
        return ConstVisitor(rootNode());
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
        NodeAllocTraits::construct(nodeAlloc(), ptr->rawPtr(),
            std::forward<Args>(args)...);
        ptr->clear();
        return ptr;
    }

    Void destroyNode(NodePtr node)
    {
        NodeAllocTraits::destroy(nodeAlloc(), node->rawPtr());
        NodeAllocTraits::deallocate(nodeAlloc(), node, 1);
    }

    template <class Visitor_>
    NodePtr copy(Visitor_ visitor)
    {
        if (!visitor) {
            return nullptr;
        }

        auto desRoot = createNode(*visitor);
        auto desParent = preBegin(Visitor(desRoot));
        for (auto srcParent = preBegin(visitor); srcParent !=
            preEnd(visitor); ++srcParent, ++desParent) {
            auto srcLeftChild = srcParent.visitor().leftChild();
            auto srcRightChild = srcParent.visitor().rightChild();
            if (srcLeftChild) {
                auto desChild = createNode(*srcLeftChild);
                desParent.visitor().node()->insertLeft(desChild);
            }
            if (srcRightChild) {
                auto desChild = createNode(*srcRightChild);
                desParent.visitor().node()->insertRight(desChild);
            }
        }
        return desRoot;
    }

    NodePtr cut(NodePtr node)
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
        node = cut(node);
        for (auto parent = postBegin(Visitor(node));
            parent != postEnd(Visitor(node)); ++parent) {
            auto parentNode = parent.visitor().node();
            auto leftChild = parentNode->leftChild();
            auto rightChild = parentNode->rightChild();
            if (leftChild) {
                parentNode->remove(leftChild);
                destroyNode(leftChild);
            }
            if (rightChild) {
                parentNode->remove(rightChild);
                destroyNode(rightChild);
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
        rootNode() = tree.cut(tree.rootNode());
        nodeAlloc() = std::move(tree.nodeAlloc());
    }

    Void moveAssign(TreeBaseType&& tree, std::false_type)
    {
        clear();
        if (nodeAlloc() == tree.nodeAlloc()) {
            rootNode() = tree.cut(tree.rootNode());
        } else {
            rootNode() = copy(tree.root());
            tree.clear();
        }
    }

    Void clear()
    {
        destroy(rootNode());
    }

    NodePtr insertLeft(NodePtr parent, NodePtr child)
    {
        NodePtr cutNode = cut(parent->leftChild());
        parent->insertLeft(child);
        return cutNode;
    }

    NodePtr insertRight(NodePtr parent, NodePtr child)
    {
        NodePtr cutNode = cut(parent->rightChild());
        parent->insertRight(child);
        return cutNode;
    }

    NodePtr remove(NodePtr node)
    {
        return cut(node);
    }

private:
    NodeAlloc   mNodeAl;
    NodePtr     mRoot;

}; /* class BinaryTreeBase */

} /* namespace det */

template <class Val, class Alloc = std::allocator<Val>>
class BinaryTree
    : public det::BinaryTreeBase<Val, Alloc>
{
    using TreeType              = BinaryTree<Val, Alloc>;
    using Base                  = det::BinaryTreeBase<Val, Alloc>;
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

    using Visitor               = typename Base::Visitor;
    using ConstVisitor          = typename Base::ConstVisitor;

    BinaryTree()
        : Base()
    {
    }

    explicit BinaryTree(const AllocatorType& alloc)
        : Base(alloc)
    {
    }

    explicit BinaryTree(ConstReference val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(val);
    }

    explicit BinaryTree(ValueType&& val,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::createNode(std::move(val));
    }

    template <class Visitor_, typename = std::enable_if_t<
        !std::is_base_of<TreeType, std::decay_t<Visitor_>>::value &&
            !std::is_convertible<ValueType, std::decay_t<Visitor_>>::value
        >
    >
    explicit BinaryTree(Visitor_ visitor,
        const AllocatorType& alloc = AllocatorType())
        : Base(alloc)
    {
        Base::rootNode() = Base::copy(visitor);
    }

    BinaryTree(const TreeType& tree)
        : Base(tree)
    {
    }

    BinaryTree(TreeType&& tree)
        : Base(std::move(tree))
    {
    }

    ~BinaryTree()
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
        return std::distance(preBegin(root()), preEnd(root()));
    }

    SizeType maxSize() const
    {
        return NodeAllocTraits::max_size(Base::nodeAlloc());
    }

    SizeType max_size() const
    {
        return maxSize();
    }

    Visitor root()
    {
        return Base::root();
    }

    ConstVisitor root() const
    {
        return Base::root();
    }

    ConstVisitor croot() const
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

    Void clear()
    {
        Base::clear();
    }

    template <class Visitor_>
    Void assign(Visitor_ root)
    {
        clear();
        Base::rootNode() = Base::copy(root);
    }

    TreeType insertLeft(ConstVisitor parent, ConstReference val)
    {
        auto child = Base::createNode(val);
        return TreeType(Base::nodeAlloc(), Base::insertLeft(parent.node(), child));
    }

    TreeType insertLeft(ConstVisitor parent, ValueType&& val)
    {
        auto child = Base::createNode(std::move(val));
        return TreeType(Base::nodeAlloc(), Base::insertLeft(parent.node(), child));
    }

    template <class... Args>
    TreeType emplaceLeft(ConstVisitor parent, Args&&... args)
    {
        auto child = Base::createNode(std::forward<Args>(args)...);
        return TreeType(Base::nodeAlloc(), Base::insertLeft(parent.node(), child));
    }

    template <class Visitor_, typename = std::enable_if_t<
        !std::is_base_of<ValueType, std::decay_t<Visitor_>>::value &&
            !std::is_convertible<ValueType, std::decay_t<Visitor_>>::value
        >
    >
    TreeType insertLeft(ConstVisitor parent, Visitor_ root)
    {
        auto child = Base::copy(root);
        return TreeType(Base::nodeAlloc(), Base::insertLeft(parent.node(), child));
    }

    TreeType insertRight(ConstVisitor parent, ConstReference val)
    {
        auto child = Base::createNode(val);
        return TreeType(Base::nodeAlloc(), Base::insertRight(parent.node(), child));
    }

    TreeType insertRight(ConstVisitor parent, ValueType&& val)
    {
        auto child = Base::createNode(std::move(val));
        return TreeType(Base::nodeAlloc(), Base::insertRight(parent.node(), child));
    }

    template <class... Args>
    TreeType emplaceRight(ConstVisitor parent, Args&&... args)
    {
        auto child = Base::createNode(std::forward<Args>(args)...);
        return TreeType(Base::nodeAlloc(), Base::insertRight(parent.node(), child));
    }

    template <class Visitor_, typename = std::enable_if_t<
        !std::is_base_of<ValueType, std::decay_t<Visitor_>>::value &&
            !std::is_convertible<ValueType, std::decay_t<Visitor_>>::value
        >
    >
    TreeType insertRight(ConstVisitor parent, Visitor_ root)
    {
        auto child = Base::copy(root);
        return TreeType(Base::nodeAlloc(), Base::insertRight(parent.node(), child));
    }

    TreeType spliceLeft(ConstVisitor parent, TreeType& tree)
    {
        return TreeType(Base::nodeAlloc(),
            Base::insertLeft(parent.node(), tree.Base::remove(tree.rootNode())));
    }

    TreeType spliceLeft(ConstVisitor parent, TreeType&& tree)
    {
        return TreeType(Base::nodeAlloc(),
            Base::insertLeft(parent.node(), tree.Base::remove(tree.rootNode())));
    }

    TreeType spliceLeft(ConstVisitor parent, TreeType& tree, ConstVisitor root)
    {
        return TreeType(Base::nodeAlloc(),
            Base::insertLeft(parent.node(), tree.Base::remove(root.node())));
    }

    TreeType spliceLeft(ConstVisitor parent, TreeType&& tree, ConstVisitor root)
    {
        return TreeType(Base::nodeAlloc(),
            Base::insertLeft(parent.node(), tree.Base::remove(root.node())));
    }

    TreeType remove(ConstVisitor node)
    {
        return TreeType(Base::nodeAlloc(), Base::remove(node.node()));
    }

    TreeType erase(ConstVisitor node)
    {
        Base::destroy(node.node());
    }

private:
    BinaryTree(const NodeAlloc& alloc, NodePtr node)
        : Base(alloc, node)
    {
    }

}; /* class BinaryTree */

template <class Val, class Alloc>
Bool operator==(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return std::equal(
        preBegin(l.croot()), preEnd(l.root()),
        preBegin(r.croot()), preEnd(r.root())
    );
}

template <class Val, class Alloc>
Bool operator!=(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return !(l == r);
}

template <class Val, class Alloc>
Bool operator<(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return std::lexicographical_compare(
        preBegin(l.croot()), preEnd(l.root()),
        preBegin(r.croot()), preEnd(r.root())
    );
}

template <class Val, class Alloc>
Bool operator>(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return r < l;
}

template <class Val, class Alloc>
Bool operator<=(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return !(l > r);
}

template <class Val, class Alloc>
Bool operator>=(const BinaryTree<Val, Alloc>& l, const BinaryTree<Val, Alloc>& r)
{
    return !(l < r);
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_BINARY_H_ */