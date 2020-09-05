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

#include "test/ad/tree/generic_ut.h"

#include "ad/mp/functional.h"
#include "ad/tree/generic.h"

using namespace ad::mp;

int main() {
    ad::UTRunner utRunner;
    utRunner.add<GenericTreeReq<TpFunc_<ad::tree::Tree>>>();
    utRunner.run();
}