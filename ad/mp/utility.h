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

#include "ad/mp/constant.h"

namespace ad
{
namespace mp
{

template <class Cond_, class Then_, class Else_>
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

} /* namespace mp */
} /* namespace ad */

#endif /* AD_MP_UTILITY_H_ */