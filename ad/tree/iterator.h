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

namespace ad
{
namespace tree
{

template <class Vertex>
Vertex leftLowestDescendant(Vertex v)
{
    for (; v.first(); v = v.first());
    return v;
}

template <class Vertex>
Vertex rightLowestDescendant(Vertex v)
{
    for (; v.last(); v = v.last());
    return v;
}

template <class Vertex>
Vertex preNext(Vertex v)
{
    if (v.first()) {
        return v.first();
    } else {
        for (; v && !v.right(); v = v.parent());
        return v ? v.right() : v;
    }
}

template <class Vertex>
Vertex prePrev(Vertex v)
{
    if (v.left()) {
        return rightLowestDescendant(v.left());
    } else {
        return v.parent();
    }
}

template <class Vertex>
Vertex postNext(Vertex v)
{
    if (v.right()) {
        return leftLowestDescendant(v.right());
    } else {
        return v.parent();
    }
}

template <class Vertex>
Vertex postPrev(Vertex v)
{
    if (v.lase()) {
        return v.last();
    } else {
        for (; v && !v.left(); v = v.parent());
        return v ? v.left() : v;
    }
}

template <class Vertex>
class Edge
{
    using EdgeType              = Edge<Vertex>;

public:
    using VertexType            = Vertex;

    Edge()
        : mFrom(), mTo()
    {
    }

    Edge(const VertexType& u, const VertexType& v)
        : mFrom(u), mTo(v)
    {
    }

    Edge(const EdgeType& edge)
        : mFrom(edge.from()), mTo(edge.to())
    {
    }

    VertexType from() const
    {
        return mFrom;
    }

    VertexType to() const
    {
        return mTo;
    }

    explicit operator Bool() const
    {
        return mFrom ? (
            mTo ? (mTo.parent() == mFrom)
                : false
        ) : false;
    }

    inline friend Bool operator==(const EdgeType& l,
        const EdgeType& r)
    {
        return l.from() == r.from() && l.to() == r.to();
    }

    inline friend Bool operator!=(const EdgeType& l,
        const EdgeType& r)
    {
        return !(l == r);
    }

private:
    VertexType  mFrom;
    VertexType  mTo;

};

template <class Vertex>
class PreVertexIterator
{
    using IterType              = PreVertexIterator<Vertex>;

public:
    using VertexType            = Vertex;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VertexType::ValueType;
    using DifferenceType        = typename VertexType::DifferenceType;
    using Pointer               = typename VertexType::Pointer;
    using Reference             = typename VertexType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PreVertexIterator()
        : mCur()
    {
    }

    template <class Vertex2>
    PreVertexIterator(const PreVertexIterator<Vertex2>& other)
        : mCur(other.vertex())
    {
    }

    template <class Vertex2>
    IterType& operator=(const PreVertexIterator<Vertex2>& other)
    {
        mCur = other.vertex();
        return *this;
    }

    VertexType vertex() const
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

    inline static IterType begin(VertexType v)
    {
        return IterType(v);
    }

    inline static IterType end(VertexType v)
    {
        return v ? IterType(preNext(rightLowestDescendant(v))) :
            IterType(v);
    }

private:
    explicit PreVertexIterator(VertexType vertex)
        : mCur(vertex)
    {
    }

    VertexType      mCur;

};

template <class Vertex1, class Vertex2>
inline Bool operator==(const PreVertexIterator<Vertex1>& l,
    const PreVertexIterator<Vertex2>& r)
{
    return l.vertex() == r.vertex();
}

template <class Vertex1, class Vertex2>
inline Bool operator!=(const PreVertexIterator<Vertex1>& l,
    const PreVertexIterator<Vertex2>& r)
{
    return !(l == r);
}

template <class Vertex>
class PostVertexIterator
{
    using IterType              = PostVertexIterator<Vertex>;

public:
    using VertexType            = Vertex;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VertexType::ValueType;
    using DifferenceType        = typename VertexType::DifferenceType;
    using Pointer               = typename VertexType::Pointer;
    using Reference             = typename VertexType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    PostVertexIterator()
        : mCur()
    {
    }

    template <class Vertex2>
    PostVertexIterator(const PostVertexIterator<Vertex2>& other)
        : mCur(other.vertex())
    {
    }

    template <class Vertex2>
    IterType& operator=(const PostVertexIterator<Vertex2>& other)
    {
        mCur = other.vertex();
        return *this;
    }

    VertexType vertex() const
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

    inline static IterType begin(VertexType v)
    {
        return v ? IterType(leftLowestDescendant(v)) : IterType(v);
    }

    inline static IterType end(VertexType v)
    {
        return v ? IterType(postNext(v)) : IterType(v);
    }

private:
    explicit PostVertexIterator(VertexType vertex)
        : mCur(vertex)
    {
    }

    VertexType      mCur;

};

template <class Vertex1, class Vertex2>
inline Bool operator==(const PostVertexIterator<Vertex1>& l,
    const PostVertexIterator<Vertex2>& r)
{
    return l.vertex() == r.vertex();
}

template <class Vertex1, class Vertex2>
inline Bool operator!=(const PostVertexIterator<Vertex1>& l,
    const PostVertexIterator<Vertex2>& r)
{
    return !(l == r);
}

template <class Vertex>
class ChildVertexIterator
{
    using IterType              = ChildVertexIterator<Vertex>;

public:
    using VertexType            = Vertex;
    using IteratorCategory      = std::forward_iterator_tag;
    using ValueType             = typename VertexType::ValueType;
    using DifferenceType        = typename VertexType::DifferenceType;
    using Pointer               = typename VertexType::Pointer;
    using Reference             = typename VertexType::Reference;

    using iterator_category     = IteratorCategory;
    using value_type            = ValueType;
    using difference_type       = DifferenceType;
    using pointer               = Pointer;
    using reference             = Reference;

    ChildVertexIterator()
        : mCur()
    {
    }

    template <class Vertex2>
    ChildVertexIterator(const ChildVertexIterator<Vertex2>& other)
        : mCur(other.vertex())
    {
    }

    template <class Vertex2>
    IterType& operator=(const ChildVertexIterator<Vertex2>& other)
    {
        mCur = other.vertex();
        return *this;
    }

    VertexType vertex() const
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
        mCur = mCur.right();
        return *this;
    }

    IterType operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    inline static IterType begin(VertexType v)
    {
        return v ? IterType(v.first()) : IterType(v);
    }

    inline static IterType end(VertexType v)
    {
        return v ? (v.last() ? IterType(v.last().right()) : IterType(v.last()))
                    : IterType(v);
    }

private:
    explicit ChildVertexIterator(VertexType vertex)
        : mCur(vertex)
    {
    }

    VertexType      mCur;

};

template <class Vertex1, class Vertex2>
inline Bool operator==(const ChildVertexIterator<Vertex1>& l,
    const ChildVertexIterator<Vertex2>& r)
{
    return l.vertex() == r.vertex();
}

template <class Vertex1, class Vertex2>
inline Bool operator!=(const ChildVertexIterator<Vertex1>& l,
    const ChildVertexIterator<Vertex2>& r)
{
    return !(l == r);
}

} /* namespace tree*/
} /* namespace ad */

#endif /* AD_TREE_ITER_H_ */