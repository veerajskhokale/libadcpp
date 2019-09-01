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

#ifndef AD_MP_ALGORITHM_H_
#define AD_MP_ALGORITHM_H_

#include "ad/mp/types.h"
#include "ad/mp/utility.h"

namespace ad
{
namespace mp
{

template <class PackT_, class UnaryPredT_>
struct forEach_;

template <
    template <class...> class PackT_,
    class... Ts_,
    class UnaryPredT_
>
struct forEach_<PackT_<Ts_...>, UnaryPredT_>
{
    using Result_ = PackT_<typename
        apply_<UnaryPredT_, Ts_>::Result_...>;
};

template <class Pack1T_, class Pack2T_, class BinaryPredT_>
struct transform_;

template <
    template <class...> class Pack1T_, class... T1s_,
    template <class...> class Pack2T_, class... T2s_,
    class BinaryPredT_
>
struct transform_<Pack1T_<T1s_...>, Pack2T_<T2s_...>, BinaryPredT_>
{
    using Result_ = Pack_<typename
        apply_<BinaryPredT_, T1s_, T2s_>::Result_...>;
};

} /* namespace mp */
} /* namespace ad */

#endif