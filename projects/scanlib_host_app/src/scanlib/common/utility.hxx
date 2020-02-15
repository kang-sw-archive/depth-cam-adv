#pragma once

#if __cplusplus < 201700L
#    include <experimental/optional>
#else
#    include <optional>
#endif

#include <stdint.h>
namespace upp {
namespace hash {

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g hash_32_fnv1a_const("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t fnv1a_32_const(
    const char* const str,
    const uint32_t    value = val_32_const) noexcept
{
    return ( str[0] == '\0' )
        ? value
        : fnv1a_32_const(
            &str[1], ( value ^ uint32_t(str[0]) ) * prime_32_const);
}

inline constexpr uint64_t
fnv1a_64(const char* const str, const uint64_t value = val_64_const) noexcept
{
    return ( str[0] == '\0' )
        ? value
        : fnv1a_64(
            &str[1], ( value ^ uint64_t(str[0]) ) * prime_64_const);
}

inline const uint32_t fnv1a_32(const void* key)
{
    const char* data = (char*) key;
    uint32_t    hash = 0x811c9dc5;
    uint32_t    prime = 0x1000193;

    for ( ; *data; ++data )
    {
        hash = hash ^ *data;
        hash *= prime;
    }

    return hash;
}
} // namespace hash

namespace binutil {
namespace impl {
//! @brief      Change single byte value into two ASCII characters
static inline uint16_t byte_to_ascii(uint8_t const c)
{
    uint8_t const lo = c >> 4;
    uint8_t const hi = c & 0xf;

    uint8_t ch[2];
    ch[0] = (lo) +'0' + ( lo > 9 )* ( 'a' - '0' - 10 );
    ch[1] = (hi) +'0' + ( hi > 9 )* ( 'a' - '0' - 10 );

    return *(uint16_t*) ch;
}

//! @brief      Change two ASCII characters into single byte.
//! @returns    byte value between 0~255. Otherwise it's invalid ascii string
static inline int ascii_to_byte(void const* c)
{
    uint8_t hi = ( (uint8_t const*) c )[0];
    uint8_t lo = ( (uint8_t const*) c )[1];
    hi = hi - '0' - ( 'a' - '0' - 10 ) * ( hi >= 'a' );
    lo = lo - '0' - ( 'a' - '0' - 10 ) * ( lo >= 'a' );
    return ( hi << 4 ) + lo + 0xfffff * ( ( hi & ~0xfu ) || ( lo & ~0xfu ) );
}
} // namespace impl

static bool atob(void const* data, void* out, size_t outSize)
{
    auto       head = reinterpret_cast<uint8_t*>( out );
    auto const end = head + outSize;

    for ( ; head != end; ++head, data = (char*) data + 2 )
    {
        *head = impl::ascii_to_byte(data);

        if ( ( *head & ~0xff ) != 0 )
            return false;
    }
    return true;
}

static size_t
btoa(char* out, size_t capacity, void const* data, size_t dataSize)
{
    size_t written = 0;

    // Make even number
    for ( capacity -= ( capacity & 1 ); capacity && dataSize; ++written )
    {
        *(uint16_t*) out = impl::byte_to_ascii(*(uint8_t*) data);
        out += 2;
        capacity -= 2;
        data = (char*) data + 1;
        --dataSize;
    }

    return written;
}

} // namespace binutil

#if __cplusplus < 201700L
template <class ty__>
using optional = std::experimental::optional<ty__>;
#else
template <class ty__>
using optional = std::optional<ty__>;
#endif

} // namespace upp