#pragma once

#include <stdint.h>
#include <string.h>

typedef struct wavFormat_t
{
    int frequence;
    int channels;
    int bits;

    struct wavFormat_t operator= (const wavFormat_t other)
    {
        frequence = other.frequence;
        channels = other.channels;
        bits = other.bits;
        return *this;
    }

} wavFormat_t;

bool convertWavFormat(
        const wavFormat_t& Iformat,
        const void* pIBuffer,
        const size_t iBytesSize,
        const wavFormat_t& Oformat,
        void* pOBuffer,
        const size_t oBytesSize);
