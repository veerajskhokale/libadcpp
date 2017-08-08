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

#include <memory>
#include <type_traits>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

#include "ad/types.h"
#include "ad/ut.h"
#include "ad/tree/iter.h"
#include "test/util.h"
#include "ad/mp/util.h"
#include "ad/mp/algorithm.h"

using namespace ad::mp;

template <class Tree>
void verifyEmpty(const Tree& tree)
{
    AD_UT_ASSERT((tree.empty()));
    AD_UT_ASSERT((!tree.root()));
    AD_UT_ASSERT((!tree.croot()));
}

template <class Tree>
void verifyNotEmpty(const Tree& tree)
{
    AD_UT_ASSERT((!tree.empty()));
    AD_UT_ASSERT((tree.root()));
    AD_UT_ASSERT((tree.croot()));
}

template <class Crawler, class Iter1>
bool verifyTraversals(Crawler root, Iter1 preFirst, Iter1 preLast)
{
    return std::equal(ad::tree::PreIterator<Crawler>::begin(root),
        ad::tree::PreIterator<Crawler>::end(root), preFirst, preLast);
}

template <class Tree, class Val, class Alloc>
std::unique_ptr<typename apply_<Tree, Val, Alloc>::Result_> makeTree0()
{
    return std::make_unique<typename apply_<Tree, Val, Alloc>::Result_>();
}

template <class Tree, class Val, class Alloc>
std::unique_ptr<typename apply_<Tree, Val, Alloc>::Result_> makeTree1()
{
    std::vector<Val> vec(1);
    std::generate(vec.begin(), vec.end(), RandomGenerator<Val>(1));

    auto tree = std::make_unique<typename
        apply_<Tree, Val, Alloc>::Result_>(vec[0]);

    AD_UT_ASSERT((verifyTraversals(tree->root(), vec.begin(), vec.end())));
    return tree;
}

template <class Tree, class Val, class Alloc>
std::unique_ptr<typename apply_<Tree, Val, Alloc>::Result_> makeTree2()
{
    std::vector<Val> vec(10);
    std::generate(vec.begin(), vec.end(), RandomGenerator<Val>(10));

    auto tree = std::make_unique<typename
        apply_<Tree, Val, Alloc>::Result_>(vec[0]);
    auto cw = tree->root();
    tree->pushBack(cw, vec[6]);
    tree->pushFront(cw, vec[3]);
    tree->insert(ad::tree::last(cw), vec[5]);
    tree->insert(ad::tree::first(cw), vec[1]);
    cw.first();
    tree->pushFront(cw, vec[2]);
    cw.right();
    tree->pushBack(cw, vec[4]);
    cw.right().right();
    tree->pushFront(cw, vec[8]);
    cw.last();
    tree->pushBack(cw, vec[9]);
    tree->insert(cw, vec[7]);

    AD_UT_ASSERT((verifyTraversals(tree->root(), vec.begin(), vec.end())));
    return tree;
}

template <class Tree, class Val, class Alloc>
std::vector<std::unique_ptr<typename
    apply_<Tree, Val, Alloc>::Result_>> makeTrees()
{
    std::vector<std::unique_ptr<typename
        apply_<Tree, Val, Alloc>::Result_>> treeVec;
    treeVec.push_back(makeTree0<Tree, Val, Alloc>());
    treeVec.push_back(makeTree1<Tree, Val, Alloc>());
    treeVec.push_back(makeTree2<Tree, Val, Alloc>());
    return treeVec;
}

template <class Tree, class Val, class Alloc>
void verifyPopBack1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    typename apply_<Tree, Val, Alloc>::Result_ treeCp(*tree);
    auto tree1 = tree->popBack(tree->root());
    AD_UT_ASSERT((treeCp == *tree));
    AD_UT_ASSERT((tree1.empty()));
}

template <class Tree, class Val, class Alloc>
void verifyPopBack2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->popBack(tree->root());
    AD_UT_ASSERT((verifyTraversals(
        tree1.root(), preVec.begin() + 6, preVec.end())));
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.begin() + 6)));

    auto tree2 = tree->popBack(tree->root());
    AD_UT_ASSERT((verifyTraversals(
        tree2.root(), preVec.begin() + 5, preVec.begin() + 6)));
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.begin() + 5)));

    auto tree3 = tree->popBack(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(
        tree3.root(), preVec.begin() + 2, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 2);
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.begin() + 4)));
}

template <class Tree, class Val, class Alloc>
void verifyPopFront1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    typename apply_<Tree, Val, Alloc>::Result_ treeCp(*tree);
    auto tree1 = tree->popFront(tree->root());
    AD_UT_ASSERT((treeCp == *tree));
    AD_UT_ASSERT((tree1.empty()));
}

template <class Tree, class Val, class Alloc>
void verifyPopFront2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->popFront(tree->root());
    AD_UT_ASSERT((verifyTraversals(
        tree1.root(), preVec.begin() + 1, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 1, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.end())));

    auto tree2 = tree->popBack(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(
        tree2.root(), preVec.begin() + 2, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 2, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.end())));
}

template <class Tree, class Val, class Alloc>
void verifyRemove1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    typename apply_<Tree, Val, Alloc>::Result_ treeCp(*tree);
    auto tree1 = tree->remove(tree->root());
    AD_UT_ASSERT((treeCp == tree1));
    AD_UT_ASSERT((tree->empty()));
}

template <class Tree, class Val, class Alloc>
void verifyRemove2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->remove(tree->root().first().right());
    AD_UT_ASSERT((verifyTraversals(
        tree1.root(), preVec.begin() + 3, preVec.begin() + 5)));
    preVec.erase(preVec.begin() + 3, preVec.begin() + 5);
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.end())));

    auto tree2 = tree->remove(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(
        tree2.root(), preVec.begin() + 1, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 1, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.end())));

    auto tree3 = tree->remove(tree->root().last());
    AD_UT_ASSERT((verifyTraversals(
        tree3.root(), preVec.begin() + 2, preVec.end())));
    preVec.erase(preVec.begin() + 2, preVec.end());
    AD_UT_ASSERT((verifyTraversals(
        tree->root(), preVec.begin(), preVec.end())));
}

template <class Tree, class Val, class Alloc>
struct AllocTypeReq
    : public ad::UnitTest
{
    void operator()()
    {
        using TreeType      = typename apply_<Tree, Val, Alloc>::Result_;
        using AllocTraits   = std::allocator_traits<Alloc>;

        AD_UT_ASSERT((std::is_same<typename AllocTraits::value_type,
            typename TreeType::ValueType>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::value_type&,
            typename TreeType::Reference>::value));
        AD_UT_ASSERT((std::is_same<const typename AllocTraits::value_type&,
            typename TreeType::ConstReference>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::pointer,
            typename TreeType::Pointer>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::const_pointer,
            typename TreeType::ConstPointer>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::size_type,
            typename TreeType::SizeType>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::difference_type,
            typename TreeType::DifferenceType>::value));

        AD_UT_ASSERT((std::is_same<typename AllocTraits::value_type,
            typename TreeType::value_type>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::value_type&,
            typename TreeType::reference>::value));
        AD_UT_ASSERT((std::is_same<const typename AllocTraits::value_type&,
            typename TreeType::const_reference>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::pointer,
            typename TreeType::pointer>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::const_pointer,
            typename TreeType::const_pointer>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::size_type,
            typename TreeType::size_type>::value));
        AD_UT_ASSERT((std::is_same<typename AllocTraits::difference_type,
            typename TreeType::difference_type>::value));
    }
};

template <class Tree, class Val, class Alloc>
struct TypeReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _uts = Pack_<
            Pred_<AllocTypeReq>
        >;
        using _utPack = typename forEach_<_uts,
            apply_<_0, Tree, Val, Alloc>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Tree, class Val, class Alloc>
struct DefaultConstructorReq
    : public ad::UnitTest
{
    void operator()()
    {
        using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
        TreeType tree;
        verifyEmpty(tree);
    }
};

template <class Tree, class Val, class Alloc>
struct AllocatorConstructorReq
    : public ad::UnitTest
{
    void operator()()
    {
        using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
        TreeType tree((Alloc()));
        verifyEmpty(tree);
    }
};

template <class Tree, class Val, class Alloc>
struct ValConstructorReq
    : ad::UnitTest
{
    void operator()()
    {
        using TreeType = typename apply_<Tree, Val, Alloc>::Result_;
        Val val1{}, val2{}, val3{};
        TreeType tree1(val1);
        TreeType tree2(std::move(val2));
        TreeType tree3(val1, Alloc());
        TreeType tree4(std::move(val3), Alloc());
        verifyNotEmpty(tree1);
        verifyNotEmpty(tree2);
        AD_UT_ASSERT((*tree1.root() == *tree1.croot()));
        AD_UT_ASSERT((*tree1.root() == val1));
        AD_UT_ASSERT((*tree2.root() == *tree2.croot()));
        AD_UT_ASSERT((*tree2.root() == val2));
        AD_UT_ASSERT((*tree3.root() == *tree3.croot()));
        AD_UT_ASSERT((*tree3.root() == val1));
        AD_UT_ASSERT((*tree4.root() == *tree4.croot()));
        AD_UT_ASSERT((*tree4.root() == val3));
    }
};

template <class Tree, class Val, class Alloc>
struct CopyConstructorReq
    : ad::UnitTest
{
    void operator()()
    {
        using TreeType = typename apply_<Tree, Val, Alloc>::Result_;

        auto trees = makeTrees<Tree, Val, Alloc>();
        for (std::size_t i = 0; i < trees.size(); ++i) {
            TreeType cp(*trees[i]);
            AD_UT_ASSERT((cp == *trees[i]));
        }
    }
};

template <class Tree, class Val, class Alloc>
struct ConstructorReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _uts = Pack_<
            Pred_<DefaultConstructorReq>,
            Pred_<AllocatorConstructorReq>,
            Pred_<ValConstructorReq>,
            Pred_<CopyConstructorReq>
        >;
        using _utPack = typename forEach_<_uts,
            apply_<_0, Tree, Val, Alloc>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Tree, class Val, class Alloc>
struct CopyAssignReq
    : ad::UnitTest
{
    void operator()()
    {
        using TreeType = typename apply_<Tree, Val, Alloc>::Result_;

        auto trees = makeTrees<Tree, Val, Alloc>();
        for (std::size_t i = 0; i < trees.size(); ++i) {
            TreeType t;
            t = *trees[i];
            AD_UT_ASSERT((t == *trees[i]));
            t = *trees[i];
            AD_UT_ASSERT((t == *trees[i]));
            if (i > 0) {
                t = *trees[i-1];
                AD_UT_ASSERT((t == *trees[i-1]));
            }
        }
        TreeType t = *trees[0];
        t = *trees[1];
        AD_UT_ASSERT((t == *trees[1]));
    }
};

template <class Tree, class Val, class Alloc>
struct AssignReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _uts = Pack_<
            Pred_<CopyAssignReq>
        >;
        using _utPack = typename forEach_<_uts,
            apply_<_0, Tree, Val, Alloc>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Tree, class Val, class Alloc>
struct GrowthReq
    : ad::UnitTest
{
    void operator()()
    {
        makeTrees<Tree, Val, Alloc>();
    }
};

template <class Tree, class Val, class Alloc>
struct PopBackReq
    : ad::UnitTest
{
    void operator()()
    {
        verifyPopBack1<Tree, Val, Alloc>();
        verifyPopBack2<Tree, Val, Alloc>();
    }
};

template <class Tree, class Val, class Alloc>
struct PopFrontReq
    : ad::UnitTest
{
    void operator()()
    {
        verifyPopFront1<Tree, Val, Alloc>();
        verifyPopFront2<Tree, Val, Alloc>();
    }
};

template <class Tree, class Val, class Alloc>
struct RemoveReq
    : ad::UnitTest
{
    void operator()()
    {
        verifyRemove1<Tree, Val, Alloc>();
        verifyRemove2<Tree, Val, Alloc>();
    }
};

template <class Tree, class Val, class Alloc>
struct PruneReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _uts = Pack_<
            Pred_<PopBackReq>,
            Pred_<PopFrontReq>,
            Pred_<RemoveReq>
        >;
        using _utPack = typename forEach_<_uts,
            apply_<_0, Tree, Val, Alloc>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Tree, class Val, class Alloc>
struct DestructorReq
    : public ad::UnitTest
{
    void operator()()
    {
        auto tree = makeTrees<Tree, Val, Alloc>();
    }
};

template <class Tree, class Val, class Alloc>
struct GenericTreeReqImpl
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _uts = Pack_<
            Pred_<TypeReq>,
            Pred_<ConstructorReq>,
            Pred_<AssignReq>,
            Pred_<GrowthReq>,
            Pred_<PruneReq>,
            Pred_<DestructorReq>
        >;
        using _utPack = typename forEach_<_uts,
            apply_<_0, Tree, Val, Alloc>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <class Tree>
struct GenericTreeReq
    : public ad::UnitTest
{
    void operator()()
    {
        ad::UTRunner utRunner;
        using _valueTypes = PackOps_::concat_<_ints,
            _uints, _intPairs, _uintPairs, _strings>::Result_;
        using _allocTypes = forEach_<_valueTypes, Pred_<std::allocator>>::Result_;
        using _utPack = typename transform_<_valueTypes,
            _allocTypes, GenericTreeReqImpl<Tree, _0, _1>>::Result_;
        UTAdder<_utPack>()(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};