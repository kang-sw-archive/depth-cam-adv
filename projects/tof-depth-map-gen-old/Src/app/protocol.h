#pragma once
#include <stdbool.h>
#include <stdint.h>

//! Packet info data type.
typedef uint32_t packetinfo_t;

#define PACKET_SIZE sizeof(packetinfo_t)
#define PACKET_ID   (0xe8 << 24)

#define PACKET_IDMASK       (~((1u << 24) - 1))
#define PACKET_STRBITMASK   (1U << 23)
#define PACKET_LENGTHMASK   (PACKET_STRBITMASK - 1)
#define PACKET_IS_PACKET(V) (((V)&PACKET_IDMASK) == PACKET_ID)
#define PACKET_IS_STR(V)    ((V)&PACKET_STRBITMASK)
#define PACKET_LENGTH(V)    ((V)&PACKET_LENGTHMASK)

#define PACKET_MAKE(IS_STR, DATALEN) (PACKET_ID | (PACKET_STRBITMASK * (IS_STR != 0)) | (PACKET_LENGTHMASK & (DATALEN)))
