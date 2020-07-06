/*
 * Copyright 2020 Veeraj S Khokale All Rights Reserved
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

#ifndef AD_MP_FUNCTIONAL_H_
#define AD_MP_FUNCTIONAL_H_

#include <ad/types.h>

namespace ad
{
namespace mp
{
namespace _functional
{

template <class T_>
using EnableIfType_ = Void;

}; /* namespace _functional */

// Logical operators
template <class T_>
struct not_
{
    using Result_ = typename T_::template not_<>::Result_;
};

template <class L_, class R_>
struct or_
{
    using Result_ = typename L_::template or_<R_>::Result_;
};

template <class L_, class R_>
struct and_
{
    using Result_ = typename L_::template and_<R_>::Result_;
};

// Comparision operators
template <class L_, class R_>
struct lt_
{
    using Result_ = typename L_::template lt_<R_>::Result_;
};

template <class L_, class R_>
struct gt_
{
    using Result_ = typename L_::template gt_<R_>::Result_;
};

template <class L_, class R_>
struct eq_
{
private:
    template <class T_, class = Void>
    struct chooseImpl_
    {
        using Result_ = typename and_<
            typename not_<typename lt_<T_, R_>::Result_>::Result_,
            typename not_<typename gt_<T_, R_>::Result_>::Result_
        >::Result_;
    };

    template <class T_>
    struct chooseImpl_<T_, _functional::EnableIfType_<typename T_::template eq_<R_>::Result_>>
    {
        using Result_ = typename T_::template eq_<R_>::Result_;
    };
public:
    using Result_ = typename chooseImpl_<L_>::Result_;
};

template <class L_, class R_>
struct ne_
{
private:
    template <class T_, class = Void>
    struct chooseImpl_
    {
        using Result_ = typename not_<typename eq_<T_, R_>::Result_>::Result_;
    };

    template <class T_>
    struct chooseImpl_<T_, _functional::EnableIfType_<typename T_::template ne_<R_>::Result_>>
    {
        using Result_ = typename T_::template ne_<R_>::Result_;
    };
public:
    using Result_ = typename chooseImpl_<L_>::Result_;
};

template <class L_, class R_>
struct le_
{
private:
    template <class T_, class = Void>
    struct chooseImpl_
    {
        using Result_ = typename or_<
            typename lt_<T_, R_>::Result_,
            typename eq_<T_, R_>::Result_
        >::Result_;
    };

    template <class T_>
    struct chooseImpl_<T_, _functional::EnableIfType_<typename T_::template le_<R_>::Result_>>
    {
        using Result_ = typename T_::template le_<R_>::Result_;
    };
public:
    using Result_ = typename chooseImpl_<L_>::Result_;
};

template <class L_, class R_>
struct ge_
{
private:
    template <class T_, class = Void>
    struct chooseImpl_
    {
        using Result_ = typename or_<
            typename gt_<T_, R_>::Result_,
            typename eq_<T_, R_>::Result_
        >::Result_;
    };

    template <class T_>
    struct chooseImpl_<T_, _functional::EnableIfType_<typename T_::template ge_<R_>::Result_>>
    {
        using Result_ = typename T_::template ge_<R_>::Result_;
    };
public:
    using Result_ = typename chooseImpl_<L_>::Result_;
};

// Arithmetic operators
template <class T_>
struct pos_
{
    using Result_ = typename T_::template pos_<>::Result_;
};

template <class T_>
struct neg_
{
    using Result_ = typename T_::template neg_<>::Result_;
};

template <class L_, class R_>
struct add_
{
    using Result_ = typename L_::template add_<R_>::Result_;
};

template <class L_, class R_>
struct sub_
{
    using Result_ = typename L_::template sub_<R_>::Result_;
};

template <class L_, class R_>
struct mul_
{
    using Result_ = typename L_::template mul_<R_>::Result_;
};

template <class L_, class R_>
struct div_
{
    using Result_ = typename L_::template div_<R_>::Result_;
};

template <class L_, class R_>
struct mod_
{
    using Result_ = typename L_::template mod_<R_>::Result_;
};

template <class T_>
struct invert_
{
    using Result_ = typename T_::template invert_<>::Result_;
};

template <class L_, class R_>
struct bitwiseAnd_
{
    using Result_ = typename L_::template bitwiseAnd_<R_>::Result_;
};

template <class L_, class R_>
struct bitwiseOr_
{
    using Result_ = typename L_::template bitwiseOr_<R_>::Result_;
};

template <class L_, class R_>
struct bitwiseXor_
{
    using Result_ = typename L_::template bitwiseXor_<R_>::Result_;
};

template <class L_, class R_>
struct lshift_
{
    using Result_ = typename L_::template lshift_<R_>::Result_;
};

template <class L_, class R_>
struct rshift_
{
    using Result_ = typename L_::template rshift_<R_>::Result_;
};

// Access operators
template <class T_, class Index_>
struct subscript_
{
    using Result_ = typename T_::template subscript_<Index_>::Result_;
};

template <class T_>
struct indirect_
{
    using Result_ = typename T_::template indirect_<>::Result_;
};

template <class T_, class Member_>
struct ptrMember_
{
    using Result_ = typename T_::template ptrMember_<Member_>::Result_;
};

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_FUNCTIONAL_H_ */