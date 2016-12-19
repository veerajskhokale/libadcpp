#ifndef AD_CORE_SORT_H_
#define AD_CORE_SORT_H_

#include <iterator>
#include <memory>
#include <algorithm>
#include <utility>

#include "ad_types.h"
#include "ad_util.h"

namespace ad
{

template <class BidirItT, class Compare>
Void insertionSort(BidirItT first, BidirItT last, Compare comp)
{
    if (first == last) {
        return;
    }
    
    auto i = first, j = first, k = first;
    typename std::iterator_traits<BidirItT>::value_type tmp;

    for (++i; i != last; ++i) {
        if (comp(*i, *first)) {
            j = i;
            tmp = std::move(*i);
            std::move_backward(first, i, ++j);
            *first = std::move(tmp);
        } else {
            j = k = i;
            --j;
            tmp = std::move(*i);
            while (comp(tmp, *j)) {
                *k = std::move(*j);
                --j;
                --k;
            }
            *k = std::move(tmp);
        }
    }
}

template <class BidirItT>
Void insertionSort(BidirItT first, BidirItT last)
{
    insertionSort(first, last, std::less<typename
        std::iterator_traits<BidirItT>::value_type>());
}

}

#endif