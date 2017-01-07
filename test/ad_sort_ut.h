#ifndef AD_TEST_SORT_UT_H_
#define AD_TEST_SORT_UT_H_

#include "ad_ut.h"

namespace ad
{

#define AD_UT_STD_INT_SORT_DEC(UTPrefix)                                    \
    AD_UT_DECLARE(UTPrefix##BorderTC);                                      \
    AD_UT_DECLARE(UTPrefix##RandomTC);                                      \
    AD_UT_DECLARE(UTPrefix##SortedTC)

#define AD_UT_STD_COMP_SORT_DEC(UTPrefix)                                   \
    AD_UT_DECLARE(UTPrefix##CompBorderTC);                                  \
    AD_UT_DECLARE(UTPrefix##CompRandomTC);                                  \
    AD_UT_DECLARE(UTPrefix##CompSortedTC)

#define AD_UT_STD_COMP_SORT_ADD(UTRunner, UTPrefix)                         \
    AD_UT_ADD(UTRunner, UTPrefix##CompBorderTC);                            \
    AD_UT_ADD(UTRunner, UTPrefix##CompRandomTC);                            \
    AD_UT_ADD(UTRunner, UTPrefix##CompSortedTC);

#define AD_UT_STD_INT_SORT_ADD(UTRunner, UTPrefix)                          \
    AD_UT_ADD(UTRunner, UTPrefix##BorderTC);                                \
    AD_UT_ADD(UTRunner, UTPrefix##RandomTC);                                \
    AD_UT_ADD(UTRunner, UTPrefix##SortedTC);

#define AD_TC_HELPER1(TC, Container, ValueType,                             \
    RandomType, Compare, Sort) do {                                         \
    TC<Container<ValueType>, RandomType>(                                   \
        Compare<ValueType>(), Sort<Container<ValueType>::iterator,          \
            Compare<ValueType>>, Compare<ValueType>()                       \
    );                                                                      \
} while (0)

#define AD_TC_HELPER2(TC, Container, ValueType, RandomType, Sort) do {      \
    TC<Container<ValueType>, RandomType>(                                   \
        std::less<ValueType>(), Sort<Container<ValueType>::iterator>        \
    );                                                                      \
} while (0)

#define AD_TC_COMP_SORT(TC, Container, Sort) do {                           \
    AD_TC_HELPER1(TC, Container, Int, Int, std::less, Sort);                \
    AD_TC_HELPER1(TC, Container, Int, Int, std::greater, Sort);             \
    AD_TC_HELPER2(TC, Container, Int, Int, Sort);                           \
    AD_TC_HELPER1(TC, Container, Int64, Int64, std::greater, Sort);         \
    AD_TC_HELPER1(TC, Container, Int64, Int64, std::less, Sort);            \
    AD_TC_HELPER2(TC, Container, Int64, Int64, Sort);                       \
    AD_TC_HELPER1(TC, Container, Uint64, Uint64, std::less, Sort);          \
    AD_TC_HELPER1(TC, Container, Uint64, Uint64, std::greater, Sort);       \
    AD_TC_HELPER2(TC, Container, Uint64, Uint64, Sort);                     \
    AD_TC_HELPER1(TC, Container, std::string, Uint64, std::less, Sort);     \
    AD_TC_HELPER1(TC, Container, std::string, Uint64, std::greater, Sort);  \
    AD_TC_HELPER2(TC, Container, std::string, Uint64, Sort);                \
} while (0)

#define AD_TC_INT_SORT(TC, Container, Sort) do {                            \
    AD_TC_HELPER2(TC, Container, Int16, Int8, Sort);                        \
    AD_TC_HELPER2(TC, Container, Int, Int8, Sort);                          \
    AD_TC_HELPER2(TC, Container, Int64, Int8, Sort);                        \
    AD_TC_HELPER2(TC, Container, Uint64, Uint8, Sort);                      \
} while (0)

#define AD_UT_STD_COMP_SORT_DEF(UTPrefix, Container, Sort)                  \
AD_UT_DEFINE(UTPrefix##CompBorderTC)                                        \
{                                                                           \
    AD_TC_COMP_SORT(internal::borderTC, Container, Sort);                   \
}                                                                           \
                                                                            \
AD_UT_DEFINE(UTPrefix##CompRandomTC)                                        \
{                                                                           \
    AD_TC_COMP_SORT(internal::randomTC, Container, Sort);                   \
}                                                                           \
                                                                            \
AD_UT_DEFINE(UTPrefix##CompSortedTC)                                        \
{                                                                           \
    AD_TC_COMP_SORT(internal::sortedTC, Container, Sort);                   \
}

#define AD_UT_STD_INT_SORT_DEF(UTPrefix, Container, Sort)                   \
AD_UT_DEFINE(UTPrefix##BorderTC)                                            \
{                                                                           \
    AD_TC_INT_SORT(internal::borderTC, Container, Sort);                    \
}                                                                           \
                                                                            \
AD_UT_DEFINE(UTPrefix##RandomTC)                                            \
{                                                                           \
    AD_TC_INT_SORT(internal::randomTC, Container, Sort);                    \
}                                                                           \
                                                                            \
AD_UT_DEFINE(UTPrefix##SortedTC)                                            \
{                                                                           \
    AD_TC_INT_SORT(internal::sortedTC, Container, Sort);                    \
}

AD_UT_STD_COMP_SORT_DEC(stdSort);
AD_UT_STD_COMP_SORT_DEC(insertionSort);
AD_UT_STD_COMP_SORT_DEC(insertionSortList);
AD_UT_STD_COMP_SORT_DEC(mergeSort);
AD_UT_STD_COMP_SORT_DEC(quickSort);
AD_UT_STD_COMP_SORT_DEC(heapSort);
AD_UT_STD_COMP_SORT_DEC(introSort);
AD_UT_STD_INT_SORT_DEC(countingSort);
AD_UT_STD_INT_SORT_DEC(countingSortList);
AD_UT_STD_INT_SORT_DEC(countingSortForwardList);
AD_UT_STD_INT_SORT_DEC(radixSort);
AD_UT_STD_INT_SORT_DEC(radixSortList);
AD_UT_STD_INT_SORT_DEC(radixSortForwardList);

}

#endif