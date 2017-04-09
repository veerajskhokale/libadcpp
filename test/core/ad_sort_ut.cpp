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
#include "ad_sort_ut.h"

int main()
{
    ad::UTRunner utRunner;

    utRunner.add<ad::AddSortUTs<ad::StdSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, false>>("StdSort");

    utRunner.add<ad::AddSortUTs<ad::StdSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, true>>("StdStableSort");

    utRunner.add<ad::AddSortUTs<ad::InsertionSort, ad::SortType::COMP_SORT,
        ad::SortIterType::BIDIR_ITER, true>>("InsertionSort");

    utRunner.add<ad::AddSortUTs<ad::InsertionSort, ad::SortType::COMP_SORT,
        ad::SortIterType::BIDIR_ITER, true>>("InsertionSort");

    utRunner.add<ad::AddSortUTs<ad::MergeSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, true>>("MergeSort");

    utRunner.add<ad::AddSortUTs<ad::QuickSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, false>>("QuickSort");

    utRunner.add<ad::AddSortUTs<ad::HeapSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, false>>("HeapSort");

    utRunner.add<ad::AddSortUTs<ad::IntroSort, ad::SortType::COMP_SORT,
        ad::SortIterType::RANDOM_ITER, false>>("IntroSort");

    utRunner.add<ad::AddSortUTs<ad::CountingSort, ad::SortType::INT_SORT,
        ad::SortIterType::FORWARD_ITER, true>>("CountingSort");

    utRunner.add<ad::AddSortUTs<ad::RadixSort, ad::SortType::INT_SORT,
        ad::SortIterType::FORWARD_ITER, false>>("RadixSort");

    utRunner.run();
    return 0;
}