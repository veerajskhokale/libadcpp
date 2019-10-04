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
namespace _types
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

} /* namespace _types */

template <Bool Val_>
using Bool_             = _types::Declare_<Bool>::Constant_<Val_>;

template <Char Val_>
using Char_             = _types::Declare_<Char>::Constant_<Val_>;

template <Byte Val_>
using Byte_             = _types::Declare_<Byte>::Constant_<Val_>;

template <Short Val_>
using Short_            = _types::Declare_<Short>::Constant_<Val_>;

template <Int Val_>
using Int_              = _types::Declare_<Int>::Constant_<Val_>;

template <Long Val_>
using Long_             = _types::Declare_<Long>::Constant_<Val_>;

template <Ubyte Val_>
using Ubyte_            = _types::Declare_<Ubyte>::Constant_<Val_>;

template <Ushort Val_>
using Ushort_           = _types::Declare_<Ushort>::Constant_<Val_>;

template <Uint Val_>
using Uint_             = _types::Declare_<Uint>::Constant_<Val_>;

template <Ulong Val_>
using Ulong_            = _types::Declare_<Ulong>::Constant_<Val_>;

template <Uint8 Val_>
using Uint8_            = _types::Declare_<Uint8>::Constant_<Val_>;

template <Uint16 Val_>
using Uint16_           = _types::Declare_<Uint16>::Constant_<Val_>;

template <Uint32 Val_>
using Uint32_           = _types::Declare_<Uint32>::Constant_<Val_>;

template <Uint64 Val_>
using Uint64_           = _types::Declare_<Uint64>::Constant_<Val_>;

template <Int8 Val_>
using Int8_             = _types::Declare_<Int8>::Constant_<Val_>;

template <Int16 Val_>
using Int16_            = _types::Declare_<Int16>::Constant_<Val_>;

template <Int32 Val_>
using Int32_            = _types::Declare_<Int32>::Constant_<Val_>;

template <Int64 Val_>
using Int64_            = _types::Declare_<Int64>::Constant_<Val_>;

template <Size Val_>
using Size_             = _types::Declare_<Size>::Constant_<Val_>;

template <PtrDiff Val_>
using PtrDiff_          = _types::Declare_<PtrDiff>::Constant_<Val_>;

using True_             = Bool_<1>;
using False_            = Bool_<0>;

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_TYPES_H_ */