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

#ifndef AD_TREE_ALGO_H_
#define AD_TREE_ALGO_H_

#include "ad/tree/iterator.h"
#include "ad/tree/visitor.h"
#include "ad/types.h"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

namespace ad {
namespace tree {

template<class ForwardVs, class T, class BinaryOp>
Void computeUp(ForwardVs root, T init, BinaryOp binaryOp) {
    for (auto [p, pe] = postIters(root); p != pe; ++p) {
        *p = std::accumulate(
            childBegin(p.visitor()), childEnd(p.visitor()), init, binaryOp);
    }
}

template<class ForwardVs, class T>
Void computeUp(ForwardVs root, T init) {
    computeUp(root, init, std::plus<T>());
}

template<class ForwardVs, class T, class BinaryOp>
Void computeDown(ForwardVs root, T rootInit, T init, BinaryOp binaryOp) {
    *root = std::move(rootInit);
    for (auto [p, pe] = preIters(root); p != pe; ++p) {
        for (auto [c, ce] = childIters(p.visitor()); c != ce; ++c) {
            *c = binaryOp(std::move(init), *p);
        }
    }
}

template<class ForwardVs, class T>
Void computeDown(ForwardVs root, T rootInit, T init) {
    computeDown(root, rootInit, init, std::plus<T>());
}

template<class InForwardVs, class OutForwardVs, class BinaryOp>
Void transformUp(InForwardVs inRoot, OutForwardVs outRoot, BinaryOp binaryOp) {
    auto inp = postBegin(inRoot);
    for (auto [outp, outpe] = postIters(outRoot); outp != outpe;
         ++inp, ++outp) {
        *outp = std::accumulate(
            childBegin(outp.visitor()), childEnd(outp.visitor()), *inp,
            binaryOp);
    }
}

template<class InForwardVs, class OutForwardVs>
Void transformUp(InForwardVs inRoot, OutForwardVs outRoot) {
    using ValueType = typename VisitorTraits<InForwardVs>::ValueType;
    transformUp(inRoot, outRoot, std::plus<ValueType>());
}

template<class InForwardVs, class T, class OutForwardVs, class BinaryOp>
Void transformDown(
    InForwardVs inRoot, T rootInit, OutForwardVs outRoot, BinaryOp binaryOp) {
    *outRoot = std::move(rootInit);
    auto inp = preBegin(inRoot);
    for (auto [outp, outpe] = preIters(outRoot); outp != outpe; ++inp, ++outp) {
        auto inc = childBegin(inp.visitor());
        for (auto [outc, outce] = childIters(outp.visitor()); outc != outce;
             ++inc, ++outc) {
            *outc = binaryOp(*inc, *outp);
        }
    }
}

template<class InForwardVs, class T, class OutForwardVs>
Void transformDown(InForwardVs inRoot, T rootInit, OutForwardVs outRoot) {
    using ValueType = typename VisitorTraits<InForwardVs>::ValueType;
    transformDown(inRoot, rootInit, outRoot, std::plus<ValueType>());
}

template<class ForwardVs>
Size count(ForwardVs root) {
    return std::distance(preBegin(root), preEnd(root));
}

template<class ForwardVs>
Size leafCount(ForwardVs root) {
    return std::distance(leafBegin(root), leafEnd(root));
}

template<class ForwardVs, class BinaryOp>
Void computeCount(ForwardVs root, BinaryOp binaryOp) {
    computeUp(root, Size(1), binaryOp);
}

template<class ForwardVs>
Void computeCount(ForwardVs root) {
    computeUp(root, Size(1));
}

template<class ForwardVs>
Size height(ForwardVs v) {
    Size h = 0;
    for (auto [c, ce] = childIters(v); c != ce; ++c) {
        h = std::max(h, height(c.visitor()) + 1);
    }
    return h;
}

template<class ForwardVs, class BinaryOp>
Void computeHeight(ForwardVs root, BinaryOp binaryOp) {
    computeUp(root, Size(0), [binaryOp](Size ph, const auto& ch) {
        return std::max(ph, binaryOp(Size(1), ch));
    });
}

template<class ForwardVs>
Void computeHeight(ForwardVs root) {
    computeHeight(root, std::plus<Size>());
}

template<class ParentVs>
Size depth(ParentVs root, ParentVs v) {
    if (v == root) { return 0; }
    Size d = 1;
    for (auto p = parentBegin(v); p.visitor() != root; ++p, ++d)
        ;
    return d;
}

template<class ForwardVs, class T, class BinaryOp>
Void computeDepth(ForwardVs root, T rootDepth, BinaryOp binaryOp) {
    computeDown(root, rootDepth, T{Size(1)}, binaryOp);
}

template<class ForwardVs, class T>
Void computeDepth(ForwardVs root, T rootDepth) {
    computeDepth(root, rootDepth, std::plus<Size>());
}

template<class ParentVs>
ParentVs LCA(ParentVs u, ParentVs v) {
    std::vector<ParentVs> upath(1, u);
    std::vector<ParentVs> vpath(1, v);

    for (auto [p, pe] = parentIters(u); p != pe; ++p) {
        upath.push_back(p.visitor());
    }
    for (auto [p, pe] = parentIters(v); p != pe; ++p) {
        vpath.push_back(p.visitor());
    }

    auto uit = upath.rbegin();
    for (auto vit = vpath.rbegin();
         uit != upath.rend() && vit != vpath.rend() && *uit == *vit;
         ++uit, ++vit)
        ;

    return *(--uit);
}

template<class ForwardVs1, class ForwardVs2>
ForwardVs2 findCorresponding(ForwardVs1 root1, ForwardVs1 u, ForwardVs2 root2) {
    auto p1 = preBegin(root1);
    for (auto [p2, p2e] = preIters(root2); p2 != p2e; ++p1, ++p2) {
        if (p1.visitor() == u) { return p2.visitor(); }
    }
    return root2;
}

template<class ParentVs, class BinaryOp>
typename VisitorTraits<ParentVs>::ValueType
    distance(ParentVs wgtu, ParentVs wgtv, BinaryOp binaryOp) {
    using ValueType = typename VisitorTraits<ParentVs>::ValueType;
    auto lca = LCA(wgtu, wgtv);
    ValueType dist{Size(0)};
    if (wgtu != lca) {
        dist = binaryOp(*wgtu, dist);
        for (auto [wgtp, wgtpe] = parentIters(wgtu); wgtp.visitor() != lca;
             ++wgtp) {
            dist = binaryOp(*wgtp, dist);
        }
    }
    if (wgtv != lca) {
        dist = binaryOp(*wgtv, dist);
        for (auto [wgtp, wgtpe] = parentIters(wgtv); wgtp.visitor() != lca;
             ++wgtp) {
            dist = binaryOp(*wgtp, dist);
        }
    }
    return dist;
}

template<class ParentVs>
typename VisitorTraits<ParentVs>::ValueType
    distance(ParentVs wgtu, ParentVs wgtv) {
    using ValueType = typename VisitorTraits<ParentVs>::ValueType;
    return distance(wgtu, wgtv, std::plus<ValueType>());
}

template<class InForwardVs, class T, class OutForwardVs, class BinaryOp>
Void distanceTransform(
    InForwardVs wgtRoot, T rootDist, OutForwardVs distRoot, BinaryOp binaryOp) {
    transformDown(wgtRoot, rootDist, distRoot, binaryOp);
}

template<class InForwardVs, class T, class OutForwardVs>
Void distanceTransform(InForwardVs wgtRoot, T rootDist, OutForwardVs distRoot) {
    using ValueType = typename VisitorTraits<InForwardVs>::ValueType;
    distanceTransform(wgtRoot, rootDist, distRoot, std::plus<ValueType>());
}

template<class InForwardVs, class OutForwardVs, class BinaryOp>
Void distanceFromTransform(
    InForwardVs wgtRoot, InForwardVs wgtSrc, OutForwardVs distRoot,
    BinaryOp binaryOp) {
    auto distSrc = findCorresponding(wgtRoot, wgtSrc, distRoot);
    distanceTransform(wgtSrc, Size(0), distSrc, binaryOp);

    auto wgtFromChild = wgtSrc;
    auto distFromChild = distSrc;
    auto wgtp = parentBegin(wgtSrc);
    for (auto distp = parentBegin(distSrc);
         distp.visitor() != distRoot.parent(); ++wgtp, ++distp) {
        *distp = binaryOp(*wgtFromChild, *distFromChild);
        auto wgtc = childBegin(wgtp.visitor());
        for (auto [distc, distce] = childIters(distp.visitor());
             distc != distce; ++wgtc, ++distc) {
            if (distc.visitor() != distFromChild) {
                distanceTransform(
                    wgtc.visitor(), binaryOp(*wgtc, *distp), distc.visitor(),
                    binaryOp);
            }
        }
        wgtFromChild = wgtp.visitor();
        distFromChild = distp.visitor();
    }
}

template<class InForwardVs, class OutForwardVs>
Void distanceFromTransform(
    InForwardVs wgtRoot, InForwardVs wgtSrc, OutForwardVs distRoot) {
    using ValueType = typename VisitorTraits<InForwardVs>::ValueType;
    distanceFromTransform(wgtRoot, wgtSrc, distRoot, std::plus<ValueType>());
}

template<class ForwardVs, class OutStream, class OutOp>
Void print(ForwardVs root, OutStream& out, OutOp outOp) {
    auto isLastChild = [](auto v) { return v.parent().last() == v; };
    std::vector<Int8> vec;
    auto [p, pe] = preIters(root);
    outOp(out, *p);
    out << '\n';
    for (++p; p != pe; ++p) {
        vec.clear();
        for (auto par = parentBegin(p.visitor()); par.visitor() != root;
             ++par) {
            vec.push_back(isLastChild(par.visitor()));
        }
        for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
            if (*it) {
                out << "     ";
            } else {
                out << "|    ";
            }
        }
        if (isLastChild(p.visitor())) {
            out << "`--- ";
        } else {
            out << "|--- ";
        }
        outOp(out, *p);
        out << '\n';
    }
}

template<class ForwardVs, class OutStream>
Void print(ForwardVs root, OutStream& out) {
    using Reference = typename VisitorTraits<ForwardVs>::Reference;
    print(root, out, [](OutStream& out, Reference v) { out << v; });
}

} /* namespace tree */
} /* namespace ad */

#endif /* AD_TREE_ALGO_H_ */