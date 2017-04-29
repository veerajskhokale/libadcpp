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

    utRunner.add<AddSortUTs<ad::StdSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, false>>("StdSort");

    utRunner.add<AddSortUTs<ad::StdSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, true>>("StdStableSort");

    utRunner.add<AddSortUTs<ad::InsertionSort, SortType::COMP_SORT,
        SortIterType::BIDIR_ITER, true>>("InsertionSort");

    utRunner.add<AddSortUTs<ad::InsertionSort, SortType::COMP_SORT,
        SortIterType::BIDIR_ITER, true>>("InsertionSort");

    utRunner.add<AddSortUTs<ad::MergeSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, true>>("MergeSort");

    utRunner.add<AddSortUTs<ad::QuickSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, false>>("QuickSort");

    utRunner.add<AddSortUTs<ad::HeapSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, false>>("HeapSort");

    utRunner.add<AddSortUTs<ad::IntroSort, SortType::COMP_SORT,
        SortIterType::RANDOM_ITER, false>>("IntroSort");

    utRunner.add<AddSortUTs<ad::CountingSort, SortType::INT_SORT,
        SortIterType::FORWARD_ITER, true>>("CountingSort");

    utRunner.add<AddSortUTs<ad::RadixSort, SortType::INT_SORT,
        SortIterType::FORWARD_ITER, false>>("RadixSort");

    utRunner.run();
    return 0;
}