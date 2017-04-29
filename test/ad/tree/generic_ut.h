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
#include "ad/types.h"
#include "ad/ut.h"
#include "ad/tree/iter.h"
#include "test/util.h"

#define VAL_TYPES int, double, std::string, std::pair<int, int>, std::vector<int>

template <template <class, class> class Tree>
struct TreeName
{
};

template <class Val>
struct ValName
{
};

template <>
struct ValName<int>
{
    static constexpr const char* name = "int";
};

template <>
struct ValName<double>
{
    static constexpr const char* name = "double";
};

template <>
struct ValName<std::string>
{
    static constexpr const char* name = "std::string";
};

template <>
struct ValName<std::pair<int, int>>
{
    static constexpr const char* name = "std::pair<int, int>";
};

template <>
struct ValName<std::vector<int>>
{
    static constexpr const char* name = "std::vector<int>";
};

template <class Alloc>
struct AllocName
{
};

template <class Val>
struct AllocName<std::allocator<Val>>
{
    static constexpr const char* name = "std::allocator";
};

template <template <template<class, class> class, class...> class UT,
    template<class, class> class Tree,
    class Val, class Alloc, class... Args>
void addUTImpl(ad::UTRunner& utRunner)
{
    utRunner.add<UT<Tree, Val, Alloc, Args...>>(
        std::string(TreeName<Tree>::name) + "<" +
        std::string(ValName<Val>::name) + ", " +
        std::string(AllocName<Alloc>::name) + ">"
    );
}

template <template <template <class, class> class, class...> class UT,
    template<class, class> class Tree>
void addUTsImpl(ad::UTRunner& utRunner)
{
}

template <template <template <class, class> class, class...> class UT,
    template<class, class> class Tree, class Val, class... Vals>
void addUTsImpl(ad::UTRunner& utRunner)
{
    addUTImpl<UT, Tree, Val, std::allocator<Val>>(utRunner);
    addUTsImpl<UT, Tree, Vals...>(utRunner);
}

template <template <template <class, class> class, class...> class UT,
    template<class, class> class Tree, class... Vals>
struct ValUTs
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        addUTsImpl<UT, Tree, Vals...>(utRunner);
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct AllocTypeReq
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        using TreeType      = Tree<Val, Alloc>;
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

template <template <class, class> class Tree>
struct TypeReq
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<ValUTs<AllocTypeReq, Tree, VAL_TYPES>>("AllocTypeReq");
        AD_UT_ASSERT(utRunner.run());
    }
};

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

template <template <class, class> class Tree,
    class Val, class Alloc>
std::unique_ptr<Tree<Val, Alloc>> makeTree0()
{
    return std::make_unique<Tree<Val, Alloc>>();
}

template <template <class, class> class Tree,
    class Val, class Alloc>
std::unique_ptr<Tree<Val, Alloc>> makeTree1()
{
    std::vector<Val> vec;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> ud(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    vec.push_back(fromRandom<Val>(ud(mt)));
    auto tree = std::make_unique<Tree<Val, Alloc>>(vec[0]);
    AD_UT_ASSERT((verifyTraversals(tree->root(), vec.begin(), vec.end())));
    return tree;
}

template <template <class, class> class Tree,
    class Val, class Alloc>
std::unique_ptr<Tree<Val, Alloc>> makeTree2()
{
    std::vector<Val> vec;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> ud(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    for (std::size_t i = 0; i < 10; ++i) {
        vec.push_back(fromRandom<Val>(ud(mt)));
    }

    auto tree = std::make_unique<Tree<Val, Alloc>>(vec[0]);
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

template <template <class, class> class Tree,
    class Val, class Alloc>
std::vector<std::unique_ptr<Tree<Val, Alloc>>> makeTrees()
{
    std::vector<std::unique_ptr<Tree<Val, Alloc>>> treeVec;
    treeVec.push_back(makeTree0<Tree, Val, Alloc>());
    treeVec.push_back(makeTree1<Tree, Val, Alloc>());
    treeVec.push_back(makeTree2<Tree, Val, Alloc>());
    return treeVec;
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyPopBack1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    Tree<Val, Alloc> treeCp(*tree);
    auto tree1 = tree->popBack(tree->root());
    AD_UT_ASSERT((treeCp == *tree));
    AD_UT_ASSERT((tree1.empty()));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyPopBack2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = Tree<Val, Alloc>;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->popBack(tree->root());
    AD_UT_ASSERT((verifyTraversals(tree1.root(), preVec.begin() + 6, preVec.end())));
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.begin() + 6)));

    auto tree2 = tree->popBack(tree->root());
    AD_UT_ASSERT((verifyTraversals(tree2.root(), preVec.begin() + 5, preVec.begin() + 6)));
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.begin() + 5)));

    auto tree3 = tree->popBack(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(tree3.root(), preVec.begin() + 2, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 2);
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.begin() + 4)));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyPopFront1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    Tree<Val, Alloc> treeCp(*tree);
    auto tree1 = tree->popFront(tree->root());
    AD_UT_ASSERT((treeCp == *tree));
    AD_UT_ASSERT((tree1.empty()));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyPopFront2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = Tree<Val, Alloc>;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->popFront(tree->root());
    AD_UT_ASSERT((verifyTraversals(tree1.root(), preVec.begin() + 1, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 1, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.end())));

    auto tree2 = tree->popBack(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(tree2.root(), preVec.begin() + 2, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 2, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.end())));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyRemove1()
{
    auto tree = makeTree1<Tree, Val, Alloc>();
    Tree<Val, Alloc> treeCp(*tree);
    auto tree1 = tree->remove(tree->root());
    AD_UT_ASSERT((treeCp == tree1));
    AD_UT_ASSERT((tree->empty()));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
void verifyRemove2()
{
    std::vector<Val> preVec;
    auto bkit = std::back_inserter(preVec);
    auto tree = makeTree2<Tree, Val, Alloc>();

    using TreeType = Tree<Val, Alloc>;
    using PreIter = ad::tree::PreIterator<typename TreeType::ConstCrawler>;

    std::copy(PreIter::begin(tree->root()), PreIter::end(tree->root()), bkit);
    auto tree1 = tree->remove(tree->root().first().right());
    AD_UT_ASSERT((verifyTraversals(tree1.root(), preVec.begin() + 3, preVec.begin() + 5)));
    preVec.erase(preVec.begin() + 3, preVec.begin() + 5);
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.end())));

    auto tree2 = tree->remove(tree->root().first());
    AD_UT_ASSERT((verifyTraversals(tree2.root(), preVec.begin() + 1, preVec.begin() + 3)));
    preVec.erase(preVec.begin() + 1, preVec.begin() + 3);
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.end())));

    auto tree3 = tree->remove(tree->root().last());
    AD_UT_ASSERT((verifyTraversals(tree3.root(), preVec.begin() + 2, preVec.end())));
    preVec.erase(preVec.begin() + 2, preVec.end());
    AD_UT_ASSERT((verifyTraversals(tree->root(), preVec.begin(), preVec.end())));
}

template <template <class, class> class Tree,
    class Val, class Alloc>
struct DefaultConstructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        using TreeType = Tree<Val, Alloc>;
        TreeType tree;
        verifyEmpty(tree);
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct AllocatorConstructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        using TreeType = Tree<Val, Alloc>;
        TreeType tree((Alloc()));
        verifyEmpty(tree);
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct ValConstructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        using TreeType = Tree<Val, Alloc>;
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

template <template <class, class> class Tree,
    class Val, class Alloc>
struct CopyConstructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        using TreeType = Tree<Val, Alloc>;

        auto trees = makeTrees<Tree, Val, Alloc>();
        for (std::size_t i = 0; i < trees.size(); ++i) {
            TreeType cp(*trees[i]);
            AD_UT_ASSERT((cp == *trees[i]));
        }
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct GrowthReqImpl
    : ad::UnitTest
{
    ad::Void operator()()
    {
        makeTrees<Tree, Val, Alloc>();
    }
};

template <template <class, class> class Tree>
struct GrowthReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<ValUTs<GrowthReqImpl,
            Tree, VAL_TYPES>>("GrowthReqImpl");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct PopBackReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        verifyPopBack1<Tree, Val, Alloc>();
        verifyPopBack2<Tree, Val, Alloc>();
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct PopFrontReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        verifyPopFront1<Tree, Val, Alloc>();
        verifyPopFront2<Tree, Val, Alloc>();
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct RemoveReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        verifyRemove1<Tree, Val, Alloc>();
        verifyRemove2<Tree, Val, Alloc>();
    }
};

template <template <class, class> class Tree>
struct PruneReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<ValUTs<PopBackReq,
            Tree, VAL_TYPES>>("PopBackReq");
        utRunner.add<ValUTs<PopFrontReq,
            Tree, VAL_TYPES>>("PopFrontReq");
            utRunner.add<ValUTs<RemoveReq,
            Tree, VAL_TYPES>>("RemoveReq");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <class, class> class Tree>
struct ConstructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<ValUTs<DefaultConstructorReq,
            Tree, VAL_TYPES>>("DefaultConstructorReq");
        utRunner.add<ValUTs<AllocatorConstructorReq,
            Tree, VAL_TYPES>>("AllocatorConstructorReq");
        utRunner.add<ValUTs<ValConstructorReq,
            Tree, VAL_TYPES>>("ValConstructorReq");
        utRunner.add<ValUTs<CopyConstructorReq,
            Tree, VAL_TYPES>>("CopyConstructorReq");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <class, class> class Tree,
    class Val, class Alloc>
struct DestructorReqImpl
    : ad::UnitTest
{
    ad::Void operator()()
    {
        auto trees = makeTrees<Tree, Val, Alloc>();
    }
};

template <template <class, class> class Tree>
struct DestructorReq
    : ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<ValUTs<DestructorReqImpl,
            Tree, VAL_TYPES>>("DestructorReqImpl");
        AD_UT_ASSERT(utRunner.run());
    }
};

template <template <class, class> class Tree>
struct GenericTreeConceptReq
    : public ad::UnitTest
{
    ad::Void operator()()
    {
        ad::UTRunner utRunner;
        utRunner.add<TypeReq<Tree>>("TypeReq");
        utRunner.add<ConstructorReq<Tree>>("ConstructorReq");
        utRunner.add<GrowthReq<Tree>>("GrowthReq");
        utRunner.add<PruneReq<Tree>>("PruneReq");
        utRunner.add<DestructorReq<Tree>>("DestructorReq");
        AD_UT_ASSERT(utRunner.run());
    }
};