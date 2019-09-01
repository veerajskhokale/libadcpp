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

#ifndef AD_MP_TYPES_H_
#define AD_MP_TYPES_H_

#include <type_traits>
#include "ad/types.h"

namespace ad
{
namespace mp
{
namespace det
{

template <class T_>
struct Declare_
{
    template <T_ Val_>
    struct Constant_
    {
        static constexpr T_ Value_ = Val_;
    };
};

} /* namespace det */

template <Bool Val_>
using Bool_             = det::Declare_<Bool>::Constant_<Val_>;

template <Char Val_>
using Char_             = det::Declare_<Char>::Constant_<Val_>;

template <Byte Val_>
using Byte_             = det::Declare_<Byte>::Constant_<Val_>;

template <Short Val_>
using Short_            = det::Declare_<Short>::Constant_<Val_>;

template <Int Val_>
using Int_              = det::Declare_<Int>::Constant_<Val_>;

template <Long Val_>
using Long_             = det::Declare_<Long>::Constant_<Val_>;

template <Ubyte Val_>
using Ubyte_            = det::Declare_<Ubyte>::Constant_<Val_>;

template <Ushort Val_>
using Ushort_           = det::Declare_<Ushort>::Constant_<Val_>;

template <Uint Val_>
using Uint_             = det::Declare_<Uint>::Constant_<Val_>;

template <Ulong Val_>
using Ulong_            = det::Declare_<Ulong>::Constant_<Val_>;

template <Uint8 Val_>
using Uint8_            = det::Declare_<Uint8>::Constant_<Val_>;

template <Uint16 Val_>
using Uint16_           = det::Declare_<Uint16>::Constant_<Val_>;

template <Uint32 Val_>
using Uint32_           = det::Declare_<Uint32>::Constant_<Val_>;

template <Uint64 Val_>
using Uint64_           = det::Declare_<Uint64>::Constant_<Val_>;

template <Int8 Val_>
using Int8_             = det::Declare_<Int8>::Constant_<Val_>;

template <Int16 Val_>
using Int16_            = det::Declare_<Int16>::Constant_<Val_>;

template <Int32 Val_>
using Int32_            = det::Declare_<Int32>::Constant_<Val_>;

template <Int64 Val_>
using Int64_            = det::Declare_<Int64>::Constant_<Val_>;

template <Size Val_>
using Size_             = det::Declare_<Size>::Constant_<Val_>;

template <PtrDiff Val_>
using PtrDiff_          = det::Declare_<PtrDiff>::Constant_<Val_>;

using True_             = Bool_<1>;
using False_            = Bool_<0>;

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_TYPES_H_ */