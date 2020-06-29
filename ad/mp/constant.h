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

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_CONSTANT_H_ */