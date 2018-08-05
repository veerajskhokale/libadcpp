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

#ifndef AD_MP_UTILITY_H_
#define AD_MP_UTILITY_H_

#include "ad/types.h"
#include "ad/mp/types.h"

namespace ad
{

namespace mp
{

template <class cond_, class Then_, class Else_>
struct if_;

template <class Then_, class Else_>
struct if_<True_, Then_, Else_>
{
    using Result_ = Then_;
};

template <class Then_, class Else_>
struct if_<False_, Then_, Else_>
{
    using Result_ = Else_;
};

template <class Index_>
struct _;

using _0    = _<Size_<0>>;
using _1    = _<Size_<1>>;
using _2    = _<Size_<2>>;
using _3    = _<Size_<3>>;
using _4    = _<Size_<4>>;
using _5    = _<Size_<5>>;

template <class...>
struct Pack_;

namespace PackOps_
{
    template <class PackT_>
    struct size_;

    template <template <class...> class PackT_, class... Ts_>
    struct size_<PackT_<Ts_...>>
    {
        using Result_ = Size_<sizeof...(Ts_)>;
    };

    template <class PackT_>
    struct front_;

    template <template <class...> class PackT_, class T_, class... Ts_>
    struct front_<PackT_<T_, Ts_...>>
    {
        using Result_ = T_;
    };

    template <class PackT_, class T_>
    struct pushFront_;

    template <template <class...> class PackT_, class... Ts_, class T_>
    struct pushFront_<PackT_<Ts_...>, T_>
    {
        using Result_ = PackT_<T_, Ts_...>;
    };

    template <class... Packs_>
    struct concat_;

    template <>
    struct concat_<>
    {
        using Result_ = Pack_<>;
    };

    template <template <class...> class PackT_, class... Ts_>
    struct concat_<PackT_<Ts_...>>
    {
        using Result_ = Pack_<Ts_...>;
    };

    template <
        template <class...> class Pack1T_, class... T1s_,
        template <class...> class Pack2T_, class... T2s_,
        class... Packs_
    >
    struct concat_<Pack1T_<T1s_...>, Pack2T_<T2s_...>, Packs_...>
    {
        using Result_ = typename concat_<
            Pack_<T1s_..., T2s_...>, Packs_...>::Result_;
    };

    template <class T_, class Count_>
    struct repeatSeq_;

    template <class T_>
    struct repeatSeq_<T_, Size_<0>>
    {
        using Result_ = Pack_<>;
    };

    template <class T_>
    struct repeatSeq_<T_, Size_<1>>
    {
        using Result_ = Pack_<T_>;
    };

    template <class T_, Size Count_>
    struct repeatSeq_<T_, Size_<Count_>>
    {
        using Result1_ = typename
            repeatSeq_<T_, Size_<Count_/2>>::Result_;
        using Result2_ = typename
            repeatSeq_<T_, Size_<Count_%2>>::Result_;
        using Result_ = typename
            concat_<Result1_, Result1_, Result2_>::Result_;
    };

    template <class PackT_, class Index_>
    struct at_;

    template <
        template <class...> class PackT_, class... Ts_,
        class Index_
    >
    struct at_<PackT_<Ts_...>, Index_>
    {
        using RepeatSeq_ = typename repeatSeq_<Void*, Index_>::Result_;

        template <class>
        struct RepeatFunc_;

        template <template <class...> class ArgPack_, class... Args_>
        struct RepeatFunc_<ArgPack_<Args_...>>
        {
            template <class AtT_>
            static AtT_ func_(Args_..., AtT_*, ...);
        };

        using Result_ = decltype(
            RepeatFunc_<RepeatSeq_>::func_((Ts_*)0 ...)
        );
    };
}

template <template <class...> class PredT_>
struct Pred_;

template <class T_>
using enableIfType_ = Void;

namespace
{
    template <class T_, class = Void>
    struct result_
    {
        using Result_ = T_;
    };

    template <class T_>
    struct result_<T_, enableIfType_<typename T_::Result_>>
    {
        using Result_ = typename T_::Result_;
    };

    template <class PackT_, class _xExp_>
    struct _xreplaceImpl_
    {
        using Result_ = _xExp_;
    };

    template <class PackT_, class Index_>
    struct _xreplaceImpl_<PackT_, _<Index_>>
    {
        using Result_ = typename PackOps_::at_<PackT_, Index_>::Result_;
    };

    template <
        class PackT_,
        template <class...> class _xPack_,
        class... _xTs_
    >
    struct _xreplaceImpl_<PackT_, _xPack_<_xTs_...>>
    {
        using Result_ = _xPack_<typename
            _xreplaceImpl_<PackT_, _xTs_>::Result_...>;
    };
}

template <class PredT_, class... Ts_>
struct apply_
{
    using Result1_ = typename _xreplaceImpl_<Pack_<Ts_...>, PredT_>::Result_;
    using Result_ = typename result_<Result1_>::Result_;
};

template <template <class...> class PredT_, class... Ts_>
struct apply_<Pred_<PredT_>, Ts_...>
{
    using Result_ = PredT_<Ts_...>;
};

}

}

#endif /* AD_MP_UTILITY_H_ */