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

namespace ad
{
namespace mp
{

template <class Pack_, template <class> class UnaryFunc_>
struct forEach_;

template <
    template <class...> class Pack_,
    class... Ts_,
    template <class> class UnaryFunc_
>
struct forEach_<Pack_<Ts_...>, UnaryFunc_>
{
    using Result_ = Pack_<typename UnaryFunc_<Ts_>::Result_...>;
};

template <class Pack1_, class Pack2_, template <class...> class OutPack_,
    template <class, class> class BinaryFunc_>
struct transform_;

template <
    template <class...> class Pack1_, class... T1s_,
    template <class...> class Pack2_, class... T2s_,
    template <class...> class OutPack_,
    template <class, class> class BinaryFunc_
>
struct transform_<Pack1_<T1s_...>, Pack2_<T2s_...>, OutPack_, BinaryFunc_>
{
    using Result_ = OutPack_<typename BinaryFunc_<T1s_, T2s_>::Result_...>;
};

} /* namespace mp */
} /* namespace ad */

#endif