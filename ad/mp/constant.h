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

#ifndef AD_MP_CONSTANT_H_
#define AD_MP_CONSTANT_H_

#include <ad/types.h>

namespace ad
{
namespace mp
{

template <class T_, T_ val_>
class Constant_
{
public:
    static constexpr T_ value_ = val_;

    template <class = Void, class Dummy_ = Void>
    struct not_;

    template <class Dummy_>
    struct not_<Void, Dummy_>
    {
        using Result_ = Constant_<decltype(!value_), !value_>;
    };

    template <class R_>
    struct or_;

    template <class R_, R_ r_>
    struct or_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ || r_), value_ || r_>;
    };

    template <class R_>
    struct and_;

    template <class R_, R_ r_>
    struct and_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ && r_), value_ && r_>;
    };

    template <class R_>
    struct lt_;

    template <class R_, R_ r_>
    struct lt_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ < r_), value_ < r_>;
    };

    template <class R_>
    struct gt_;

    template <class R_, R_ r_>
    struct gt_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ > r_), (value_ > r_)>;
    };

    template <class R_>
    struct eq_;

    template <class R_, R_ r_>
    struct eq_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ == r_), value_ == r_>;
    };

    template <class R_>
    struct ne_;

    template <class R_, R_ r_>
    struct ne_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ != r_), value_ != r_>;
    };

    template <class R_>
    struct le_;

    template <class R_, R_ r_>
    struct le_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ <= r_), value_ <= r_>;
    };

    template <class R_>
    struct ge_;

    template <class R_, R_ r_>
    struct ge_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ >= r_), value_ >= r_>;
    };

    template <class = Void, class Dummy_ = Void>
    struct pos_;

    template <class Dummy_>
    struct pos_<Void, Dummy_>
    {
        using Result_ = Constant_<decltype(+value_), +value_>;
    };

    template <class = Void, class Dummy_ = Void>
    struct neg_;

    template <class Dummy_>
    struct neg_<Void, Dummy_>
    {
        using Result_ = Constant_<decltype(-value_), -value_>;
    };

    template <class R_>
    struct add_;

    template <class R_, R_ r_>
    struct add_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ + r_), value_ + r_>;
    };

    template <class R_>
    struct sub_;

    template <class R_, R_ r_>
    struct sub_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ - r_), value_ - r_>;
    };

    template <class R_>
    struct mul_;

    template <class R_, R_ r_>
    struct mul_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ * r_), value_ * r_>;
    };

    template <class R_>
    struct div_;

    template <class R_, R_ r_>
    struct div_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ / r_), value_ / r_>;
    };

    template <class R_>
    struct mod_;

    template <class R_, R_ r_>
    struct mod_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ % r_), value_ % r_>;
    };

    template <class = Void, class Dummy_ = Void>
    struct invert_;

    template <class Dummy_>
    struct invert_<Void, Dummy_>
    {
        using Result_ = Constant_<decltype(~value_), ~value_>;
    };

    template <class R_>
    struct bitwiseAnd_;

    template <class R_, R_ r_>
    struct bitwiseAnd_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ & r_), value_ & r_>;
    };

    template <class R_>
    struct bitwiseOr_;

    template <class R_, R_ r_>
    struct bitwiseOr_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ | r_), value_ | r_>;
    };

    template <class R_>
    struct bitwiseXor_;

    template <class R_, R_ r_>
    struct bitwiseXor_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ ^ r_), value_ ^ r_>;
    };

    template <class R_>
    struct lshift_;

    template <class R_, R_ r_>
    struct lshift_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ << r_), value_ << r_>;
    };

    template <class R_>
    struct rshift_;

    template <class R_, R_ r_>
    struct rshift_<Constant_<R_, r_>>
    {
        using Result_ = Constant_<decltype(value_ >> r_), (value_ >> r_)>;
    };

}; /* class Constant_ */

template <Bool val_>
using Bool_             = Constant_<Bool, val_>;

template <Char val_>
using Char_             = Constant_<Char, val_>;

template <Byte val_>
using Byte_             = Constant_<Byte, val_>;

template <Short val_>
using Short_            = Constant_<Short, val_>;

template <Int val_>
using Int_              = Constant_<Int, val_>;

template <Long val_>
using Long_             = Constant_<Long, val_>;

template <Ubyte val_>
using Ubyte_            = Constant_<Ubyte, val_>;

template <Ushort val_>
using Ushort_           = Constant_<Ushort, val_>;

template <Uint val_>
using Uint_             = Constant_<Uint, val_>;

template <Ulong val_>
using Ulong_            = Constant_<Ulong, val_>;

template <Uint8 val_>
using Uint8_            = Constant_<Uint8, val_>;

template <Uint16 val_>
using Uint16_           = Constant_<Uint16, val_>;

template <Uint32 val_>
using Uint32_           = Constant_<Uint32, val_>;

template <Uint64 val_>
using Uint64_           = Constant_<Uint64, val_>;

template <Int8 val_>
using Int8_             = Constant_<Int8, val_>;

template <Int16 val_>
using Int16_            = Constant_<Int16, val_>;

template <Int32 val_>
using Int32_            = Constant_<Int32, val_>;

template <Int64 val_>
using Int64_            = Constant_<Int64, val_>;

template <Size val_>
using Size_             = Constant_<Size, val_>;

template <PtrDiff val_>
using PtrDiff_          = Constant_<PtrDiff, val_>;

using True_             = Bool_<1>;
using False_            = Bool_<0>;

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_CONSTANT_H_ */