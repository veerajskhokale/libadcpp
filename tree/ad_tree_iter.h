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
#include "ad_types.h"

namespace ad
{
namespace tree
{

template <class Crawler>
inline Crawler parent(Crawler cw)
{
    return cw.parent();
}

template <class Crawler>
inline Crawler first(Crawler cw)
{
    return cw.first();
}

template <class Crawler>
inline Crawler last(Crawler cw)
{
    return cw.last();
}

template <class Crawler>
inline Crawler left(Crawler cw)
{
    return cw.left();
}

template <class Crawler>
inline Crawler right(Crawler cw)
{
    return cw.right();
}

template <class Crawler>
Crawler leftLowestDescendant(Crawler cw)
{
    while (first(cw)) {
        cw.first();
    }
    return cw;
}

template <class Crawler>
Crawler rightLowestDescendant(Crawler cw)
{
    while (last(cw)) {
        cw.last();
    }
    return cw;
}

template <class Crawler>
Crawler preNext(Crawler cw)
{
    if (first(cw)) {
        return first(cw);
    } else {
        while (cw && !right(cw)) {
            cw.parent();
        }
        return cw ? right(cw) : cw;
    }
}

template <class Crawler>
Crawler prePrev(Crawler cw)
{
    if (left(cw)) {
        return rightLowestDescendant(left(cw));
    } else {
        return parent(cw);
    }
}

template <class Crawler>
Crawler postNext(Crawler cw)
{
    if (right(cw)) {
        return leftLowestDescendant(right(cw));
    } else {
        return parent(cw);
    }
}

template <class Crawler>
Crawler postPrev(Crawler cw)
{
    if (last(cw)) {
        return last(cw);
    } else {
        while (cw && !left(cw)) {
            cw.parent();
        }
        return cw ? left(cw) : cw;
    }
}

template <class Crawler>
class PreIterator
{
    using IterType              = PreIterator<Crawler>;

public:
    using CrawlerType           = Crawler;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename CrawlerType::ValueType;
    using DifferenceType        = typename CrawlerType::DifferenceType;
    using Pointer               = typename CrawlerType::Pointer;
    using Reference             = typename CrawlerType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PreIterator()
        : mCur()
    {
    }

    template <class Crawler2>
    PreIterator(const PreIterator<Crawler2>& other)
        : mCur(other.crawler())
    {
    }

    template <class Crawler2>
    IterType& operator=(const PreIterator<Crawler2>& other)
    {
        mCur = other.crawler();
        return *this;
    }

    CrawlerType crawler() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return std::addressof(operator*());
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

    inline static IterType begin(CrawlerType cw)
    {
        return IterType(cw);
    }

    inline static IterType end(CrawlerType cw)
    {
        return cw ? IterType(preNext(rightLowestDescendant(cw))) :
            IterType(cw);
    }

private:
    explicit PreIterator(CrawlerType crawler)
        : mCur(crawler)
    {
    }

    CrawlerType      mCur;

};

template <class Crawler1, class Crawler2>
inline Bool operator==(const PreIterator<Crawler1>& l,
    const PreIterator<Crawler2>& r)
{
    return l.crawler() == r.crawler();
}

template <class Crawler1, class Crawler2>
inline Bool operator!=(const PreIterator<Crawler1>& l,
    const PreIterator<Crawler2>& r)
{
    return !(l == r);
}

template <class Crawler>
class PostIterator
{
    using IterType              = PostIterator<Crawler>;

public:
    using CrawlerType           = Crawler;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename CrawlerType::ValueType;
    using DifferenceType        = typename CrawlerType::DifferenceType;
    using Pointer               = typename CrawlerType::Pointer;
    using Reference             = typename CrawlerType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PostIterator()
        : mCur()
    {
    }

    template <class Crawler2>
    PostIterator(const PostIterator<Crawler2>& other)
        : mCur(other.crawler())
    {
    }

    template <class Crawler2>
    IterType& operator=(const PostIterator<Crawler2>& other)
    {
        mCur = other.crawler();
        return *this;
    }

    CrawlerType crawler() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return std::addressof(operator*());
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

    inline static IterType begin(CrawlerType cw)
    {
        return cw ? IterType(leftLowestDescendant(cw)) : IterType(cw);
    }

    inline static IterType end(CrawlerType cw)
    {
        return cw ? IterType(postNext(cw)) : IterType(cw);
    }

private:
    explicit PostIterator(CrawlerType crawler)
        : mCur(crawler)
    {
    }

    CrawlerType      mCur;

};

template <class Crawler1, class Crawler2>
inline Bool operator==(const PostIterator<Crawler1>& l,
    const PostIterator<Crawler2>& r)
{
    return l.crawler() == r.crawler();
}

template <class Crawler1, class Crawler2>
inline Bool operator!=(const PostIterator<Crawler1>& l,
    const PostIterator<Crawler2>& r)
{
    return !(l == r);
}

template <class Crawler>
class ChildIterator
{
    using IterType              = ChildIterator<Crawler>;

public:
    using CrawlerType           = Crawler;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename CrawlerType::ValueType;
    using DifferenceType        = typename CrawlerType::DifferenceType;
    using Pointer               = typename CrawlerType::Pointer;
    using Reference             = typename CrawlerType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    ChildIterator()
        : mCur()
    {
    }

    template <class Crawler2>
    ChildIterator(const ChildIterator<Crawler2>& other)
        : mCur(other.crawler())
    {
    }

    template <class Crawler2>
    IterType& operator=(const ChildIterator<Crawler2>& other)
    {
        mCur = other.crawler();
        return *this;
    }

    CrawlerType crawler() const
    {
        return mCur;
    }

    Reference operator*() const
    {
        return *mCur;
    }

    Pointer operator->() const
    {
        return std::addressof(operator*());
    }

    IterType& operator++()
    {
        mCur.right();
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(CrawlerType cw)
    {
        return cw ? IterType(first(cw)) : IterType(cw);
    }

    inline static IterType end(CrawlerType cw)
    {
        return cw ? (cw.last() ? IterType(right(cw)) :
            IterType(cw)) : IterType(cw);
    }

private:
    explicit ChildIterator(CrawlerType crawler)
        : mCur(crawler)
    {
    }

    CrawlerType      mCur;

};

template <class Crawler1, class Crawler2>
inline Bool operator==(const ChildIterator<Crawler1>& l,
    const ChildIterator<Crawler2>& r)
{
    return l.crawler() == r.crawler();
}

template <class Crawler1, class Crawler2>
inline Bool operator!=(const ChildIterator<Crawler1>& l,
    const ChildIterator<Crawler2>& r)
{
    return !(l == r);
}

}
}

#endif