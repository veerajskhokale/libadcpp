/**
 * @file    ad_types.h
 * @brief   Type definitions used in libadcpp
 */

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

/**
 * @brief   Void type
 *
 * Equivalent to C++ void data type
 */
using Void      = void;

/**
 * @brief   Boolean type
 *
 * Equivalent to C++ bool data type
 */
using Bool      = bool;

/**
 * @brief   Character type
 *
 * Equivalent to C++ char data type
 */
using Char      = char;

/**
 * @brief   Fastest integer type with width atleast 8 bits
 *
 * This is the fastest integer type of size atleast
 * 8 bits provided by the platform. However its size
 * can be greater than 8 bits. Use Int8 if integer
 * of width exactly 8 bits is required.
 */
using Byte      = int_fast8_t;

/**
 * @brief   Fastest integer type with width atleast 16 bits
 *
 * This is the fastest integer type of size atleast
 * 16 bits provided by the platform. However its size
 * can be greater than 16 bits. Use Int16 if integer
 * of width exactly 16 bits is required.
 */
using Short     = int_fast16_t;

/**
 * @brief   Fastest integer type with width atleast 32 bits
 *
 * This is the fastest integer type of size atleast
 * 32 bits provided by the platform. However its size
 * can be greater than 32 bits. Use Int32 if integer
 * of width exactly 32 bits is required.
 */
using Int       = int_fast32_t;

/**
 * @brief   Fastest integer type with width atleast 64 bits
 *
 * This is the fastest integer type of size atleast
 * 64 bits provided by the platform. However its size
 * can be greater than 64 bits. Use Int64 if integer
 * of width exactly 64 bits is required.
 */
using Long      = int_fast64_t;

/**
 * @brief   Fastest unsigned integer type with width atleast 8 bits
 *
 * This is the fastest unsigned integer type of size
 * atleast 8 bits provided by the platform. However
 * its size can be greater than 8 bits. Use Uint8 if
 * unsigned integer of width exactly 8 bits is required.
 */
using Ubyte     = uint_fast8_t;

/**
 * @brief   Fastest unsigned integer type with width atleast 16 bits
 *
 * This is the fastest unsigned integer type of size
 * atleast 16 bits provided by the platform. However
 * its size can be greater than 16 bits. Use Uint16 if
 * unsigned integer of width exactly 16 bits is required.
 */
using Ushort    = uint_fast16_t;

/**
 * @brief   Fastest unsigned integer type with width atleast 32 bits
 *
 * This is the fastest unsigned integer type of size
 * atleast 32 bits provided by the platform. However
 * its size can be greater than 32 bits. Use Uint32 if
 * unsigned integer of width exactly 32 bits is required.
 */
using Uint      = uint_fast32_t;

/**
 * @brief   Fastest unsigned integer type with width atleast 64 bits
 *
 * This is the fastest unsigned integer type of size
 * atleast 64 bits provided by the platform. However
 * its size can be greater than 64 bits. Use Uint64 if
 * unsigned integer of width exactly 64 bits is required.
 */
using Ulong     = uint_fast64_t;

/**
 * @brief   Integer type with width exactly 8 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Byte type.
 */
using Int8      = int8_t;

/**
 * @brief   Integer type with width exactly 16 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Short type.
 */
using Int16     = int16_t;

/**
 * @brief   Integer type with width exactly 32 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Int type.
 */
using Int32     = int32_t;

/**
 * @brief   Integer type with width exactly 64 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Long type.
 */
using Int64     = int64_t;

/**
 * @brief   Unsigned integer type with width exactly 8 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Ubyte type.
 */
using Uint8     = uint8_t;

/**
 * @brief   Unsigned integer type with width exactly 16 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Ushort type.
 */
using Uint16    = uint16_t;

/**
 * @brief   Unsigned integer type with width exactly 32 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Uint type.
 */
using Uint32    = uint32_t;

/**
 * @brief   Unsigned integer type with width exactly 64 bits
 *
 * Use this type only if exact size semantics are
 * needed, else use the faster Ulong type.
 */
using Uint64    = uint64_t;

/**
 * @brief   Long width floating point type
 *
 * Equivalent to C++ double data type
 */
using Double    = double;

/**
 * @brief   Floating point type
 *
 * Equivalent to C++ float data type
 */
using Float     = float;

/**
 * @brief   Unsigned integer type
 *
 * A type which can store the maximum size of a theoretically
 * possible object of any type (including array). A type whose
 * size cannot be represented by this type is ill-formed.
 */
using Size      = std::size_t;

/**
 * @brief   Signed integer type
 *
 * This type is used for pointer arithmetic and array indexing,
 * if negative values are possible. Other types used for pointer
 * arithmetic may fail.
 */
using PtrDiff   = std::ptrdiff_t;

} /* namespace ad */

#endif /* AD_CORE_TYPES_H_ */