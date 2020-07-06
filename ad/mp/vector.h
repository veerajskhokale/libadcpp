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

#ifndef AD_MP_VECTOR_H_
#define AD_MP_VECTOR_H_

#include <ad/mp/constant.h>

namespace ad
{
namespace mp
{

template <class... Ts_>
class Vector_
{
public:
    template <class = Void, class Dummy_ = Void>
    struct size_;

    template <class Dummy_>
    struct size_<Void, Dummy_>
    {
        using Result_ = Size_<sizeof...(Ts_)>;
    };

    template <class = Void, class Dummy_ = Void>
    struct empty_;

    template <class Dummy_>
    struct empty_<Void, Dummy_>
    {
        using Result_ = Bool_<size_<>::Result_::value_ == 0>;
    };

    template <class = Void, class Dummy_ = Void>
    struct front_;

    template <class Dummy_>
    struct front_<Void, Dummy_>
    {
    private:
        template <class TT_, class... TTs_>
        struct helper_
        {
            using Result_ = TT_;
        };
    public:
        using Result_ = typename helper_<Ts_...>::Result_;
    };

    template <class Index_>
    struct at_;

    template <Size index_>
    struct at_<Size_<index_>>
    {

    };

    template <class T_>
    struct pushFront_
    {
        using Result_ = Vector_<T_, Ts_...>;
    };

    template <class Vec_>
    struct concat_;

    template <class... TTs_>
    struct concat_<Vector_<TTs_...>>
    {
        using Result_ = Vector_<Ts_..., TTs_...>;
    };

}; /* class Vector_ */

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_VECTOR_H_ */