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

namespace ad {
namespace mp {

template <class... Ts_>
class Vector_ {
  public:
    template <class = Void, class Dummy_ = Void>
    struct size_;

    template <class Dummy_>
    struct size_<Void, Dummy_> {
        using Result_ = Size_<sizeof...(Ts_)>;
    };

    template <class = Void, class Dummy_ = Void>
    struct empty_;

    template <class Dummy_>
    struct empty_<Void, Dummy_> {
        using Result_ = Bool_<size_<>::Result_::value_ == 0>;
    };

    template <class = Void, class Dummy_ = Void>
    struct front_;

    template <class Dummy_>
    struct front_<Void, Dummy_> {
      private:
        template <class TT_, class... TTs_>
        struct helper_ {
            using Result_ = TT_;
        };

      public:
        using Result_ = typename helper_<Ts_...>::Result_;
    };

  private:
    template <class T_, class Count_>
    struct repeatSeq_;

    template <class T_>
    struct repeatSeq_<T_, Size_<0>> {
        using Result_ = Vector_<>;
    };

    template <class T_>
    struct repeatSeq_<T_, Size_<1>> {
        using Result_ = Vector_<T_>;
    };

    template <class T_, Size Count_>
    struct repeatSeq_<T_, Size_<Count_>> {
        using Result1_ = typename repeatSeq_<T_, Size_<Count_ / 2>>::Result_;
        using Result2_ = typename repeatSeq_<T_, Size_<Count_ % 2>>::Result_;
        using Result_ = typename Result1_::template concat_<
            Result1_>::Result_ ::template concat_<Result2_>::Result_;
    };

  public:
    template <class Index_>
    struct at_;

    template <Size index_>
    struct at_<Size_<index_>> {
        using RepeatSeq_ = typename repeatSeq_<Void*, Size_<index_>>::Result_;

        template <class>
        struct RepeatFunc_;

        template <template <class...> class ArgPack_, class... Args_>
        struct RepeatFunc_<ArgPack_<Args_...>> {
            template <class AtT_>
            static AtT_ func_(Args_..., AtT_*, ...);
        };

        using Result_ = decltype(RepeatFunc_<RepeatSeq_>::func_((Ts_*)0 ...));
    };

    template <class T_>
    struct pushFront_ {
        using Result_ = Vector_<T_, Ts_...>;
    };

    template <class Vec_>
    struct concat_;

    template <class... TTs_>
    struct concat_<Vector_<TTs_...>> {
        using Result_ = Vector_<Ts_..., TTs_...>;
    };

}; /* class Vector_ */

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_VECTOR_H_ */