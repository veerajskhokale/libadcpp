#ifndef AD_CORE_TYPES_H_
#define AD_CORE_TYPES_H_

#include <cstdint>
#include <cstddef>

namespace ad
{

#define AD_RESET            "\033[0m"
#define AD_BLACK            "\033[30m"      
#define AD_RED              "\033[31m"      
#define AD_GREEN            "\033[32m"      
#define AD_YELLOW           "\033[33m"      
#define AD_BLUE             "\033[34m"      
#define AD_MAGENTA          "\033[35m"      
#define AD_CYAN             "\033[36m"      
#define AD_WHITE            "\033[37m"      
#define AD_BOLD_BLACK       "\033[1m\033[30m"      
#define AD_BOLD_RED         "\033[1m\033[31m"      
#define AD_BOLD_GREEN       "\033[1m\033[32m"      
#define AD_BOLD_YELLOW      "\033[1m\033[33m"      
#define AD_BOLD_BLUE        "\033[1m\033[34m"      
#define AD_BOLD_MAGENTA     "\033[1m\033[35m"      
#define AD_BOLD_CYAN        "\033[1m\033[36m"      
#define AD_BOLD_WHITE       "\033[1m\033[37m"

using Void      = void;
using Bool      = bool;

using Char      = char;

using Byte      = int_fast8_t;
using Short     = int_fast16_t;
using Int       = int_fast32_t;
using Long      = int_fast64_t;

using Ubyte     = uint_fast8_t;
using Ushort    = uint_fast16_t;
using Uint      = uint_fast32_t;
using Ulong     = uint_fast64_t;

using Int8      = int8_t;
using Int16     = int16_t;
using Int32     = int32_t;
using Int64     = int64_t;

using Uint8     = uint8_t;
using Uint16    = uint16_t;
using Uint32    = uint32_t;
using Uint64    = uint64_t;

using Double    = double;
using Float     = float;

using Size      = std::size_t;
using PtrDiff   = std::ptrdiff_t;

}

#endif