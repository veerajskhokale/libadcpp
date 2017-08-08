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

#include <iostream>
#include "test/ad/sort_ut.h"

int main()
{
    ad::UTRunner utRunner;

    utRunner.add<SortReq<ad::StdSort, RandomAccess,
        ComparisionSort, False_>>();

    utRunner.add<SortReq<ad::StdStableSort, RandomAccess,
        ComparisionSort, True_>>();

    utRunner.add<SortReq<ad::InsertionSort, BidirAccess,
        ComparisionSort, True_>>();

    utRunner.add<SortReq<ad::MergeSort, RandomAccess,
        ComparisionSort, True_>>();

    utRunner.add<SortReq<ad::QuickSort, RandomAccess,
        ComparisionSort, False_>>();

    utRunner.add<SortReq<ad::HeapSort, RandomAccess,
        ComparisionSort, False_>>();

    utRunner.add<SortReq<ad::IntroSort, RandomAccess,
        ComparisionSort, False_>>();

    utRunner.add<SortReq<ad::CountingSort, ForwardAccess,
        IntegerSort, True_>>();

    utRunner.add<SortReq<ad::RadixSort, ForwardAccess,
        IntegerSort, False_>>();

    utRunner.run();
    return 0;
}