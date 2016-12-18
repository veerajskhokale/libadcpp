#ifndef AD_CORE_UTIL_H_
#define AD_CORE_UTIL_H_

#include <utility>
#include <type_traits>

#include "ad_types.h"

namespace ad
{

template <class Object, class... Args>
std::decay_t<Object> makeObject(Args&&... args)
{
    return std::decay_t<Object>(std::forward<Args>(args)...);
}

}

#endif