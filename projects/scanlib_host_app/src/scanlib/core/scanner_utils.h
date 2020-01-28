//! Defines scanned data header.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#define SCAN_DATA_FORMAT_EXTENSION "dpta"
#define SCAN_DATA_FORMAT_HEADER    "dpta"

#pragma pack( push, 2 )
typedef struct ScanDataPixel {
    long           Q9_22_DEPTH;
    unsigned short UQ_12_4_AMP;
} ScanDataPixelType;

typedef struct ScanDataHeader {
    char          dpta[4]; // Must be 'dpta'
    unsigned long CHUNK_SIZE;
    unsigned long DATA_SIZE;
    unsigned long ELEMENT_SIZE;
    unsigned long NUM_PIXELS;
    unsigned long WIDTH;
    unsigned long HEIGHT;
    float         ASPECT_RATIO;
} ScanDataHeaderType;
#pragma pack( pop )

static inline void ScanDataWriteTo( FILE* out_strm, void const* pixel,
                                    unsigned long width, unsigned long height, float aspect )
{
    ScanDataHeaderType h;
    h.dpta[0] = 'd';
    h.dpta[1] = 'p';
    h.dpta[2] = 't';
    h.dpta[3] = 'a';

    h.ELEMENT_SIZE = sizeof( ScanDataPixelType );
    h.NUM_PIXELS   = width * height;
    h.WIDTH        = width;
    h.HEIGHT       = height;
    h.DATA_SIZE    = h.NUM_PIXELS * h.ELEMENT_SIZE;
    h.CHUNK_SIZE   = h.DATA_SIZE + sizeof( ScanDataHeaderType ) - 8;
    h.ASPECT_RATIO = aspect;

    fwrite( &h, 1, sizeof( h ), out_strm );
    fwrite( pixel, h.ELEMENT_SIZE, h.NUM_PIXELS, out_strm );
}

static inline bool ScanDataReadFrom( FILE* in_strm, ScanDataPixelType** outPixels,
                                     ScanDataHeaderType* outDesc )
{
    if ( fread( outDesc, sizeof( ScanDataHeaderType ), 1, in_strm ) != 1 ) {
        return false;
    }

    // ID mismatch
    if ( memcmp( outDesc, SCAN_DATA_FORMAT_HEADER, 4 ) ) {
        return false;
    }

    *outPixels = (ScanDataPixelType*)malloc( outDesc->DATA_SIZE );
    if ( *outPixels == NULL ) {
        return false;
    }

    if ( fread( *outPixels, outDesc->ELEMENT_SIZE, outDesc->NUM_PIXELS, in_strm ) != outDesc->NUM_PIXELS ) {
        free( *outPixels );
        return false;
    }

    return true;
}

#ifdef __cplusplus
}
#    include <iostream>

namespace scanlib {
static bool ScanDataReadFrom( std::istream& strm, ScanDataPixelType** outPixels, ScanDataHeaderType* outDesc )
{
    if ( strm.read( (char*)outDesc, sizeof( ScanDataHeaderType ) ).gcount() != sizeof( ScanDataHeaderType ) ) {
        return false;
    }

    if ( memcmp( outDesc, SCAN_DATA_FORMAT_HEADER, 4 ) ) {
        return false;
    }

    *outPixels = (ScanDataPixelType*)malloc( outDesc->DATA_SIZE );
    if ( *outPixels == NULL ) {
        return false;
    }

    if ( strm.read( (char*)*outPixels, outDesc->ELEMENT_SIZE * outDesc->NUM_PIXELS ).gcount() != outDesc->ELEMENT_SIZE * outDesc->NUM_PIXELS ) {
        free( *outPixels );
        return false;
    }

    return true;
}

} // namespace scanlib
#endif