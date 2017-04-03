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