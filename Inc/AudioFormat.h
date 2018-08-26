#pragma once

#include <stdint.h>
#include <string.h>

#define WAV_FIFF_SIZE (44)

typedef struct audioFormat_t
{
    int frequence;
    int channels;
    int bits;

    struct audioFormat_t operator= (const audioFormat_t other)
    {
        frequence = other.frequence;
        channels = other.channels;
        bits = other.bits;
        return *this;
    }

} audioFormat_t;

typedef struct wavFormat_t : public audioFormat_t
{
    size_t totalSize;

    struct wavFormat_t operator =( const struct wavFormat_t other)
    {
        this->audioFormat_t::operator =(other);
        totalSize = other.totalSize;
        return *this;
    }
} wavFormat_t;

bool convertWavFormat(
        const audioFormat_t& Iformat,
        const void* pIBuffer,
        const size_t iBytesSize,
        const audioFormat_t& Oformat,
        void* pOBuffer,
        const size_t oBytesSize);

/**
 * @brief read WAV format 
 * from fixed 44 Bytes front of wav file
 * @param[out] format
 * @param[in]  44 Bytes front of wav file
 * @return false if check format false
 */
bool readWavFormat(
        wavFormat_t& format,
        const void* ptr);


int wavTime2Size(
        const audioFormat_t& format,
        int ms);

int wavSize2Time(
        const audioFormat_t& format,
        int size);
