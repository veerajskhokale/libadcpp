/*
 * Copyright 2019 Veeraj S Khokale All Rights Reserved
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

#ifndef AD_TREE_VISITOR_H_
#define AD_TREE_VISITOR_H_

namespace ad
{
namespace tree
{

struct StructureConstruct {};

struct ParentVisitorTag {};
struct ForwardVisitorTag : public ParentVisitorTag {};
struct BidirectionalVisitorTag : public ForwardVisitorTag {};
struct BinaryVisitorTag : public BidirectionalVisitorTag {};
struct CompleteBinaryVisitorTag : public BinaryVisitorTag {};

template <class Visitor>
struct VisitorTraits
{
    using VisitorType           = Visitor;
    using VisitorCategory       = typename Visitor::VisitorCategory;
    using ValueType             = typename Visitor::ValueType;
    using Reference             = typename Visitor::Reference;
    using Pointer               = typename Visitor::Pointer;
    using DifferenceType        = typename Visitor::DifferenceType;
};

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_VISITOR_H_ */