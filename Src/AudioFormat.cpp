#include "AudioFormat.h"

bool convertWavFormat(
        const audioFormat_t& Iformat,
        const void* pIBuffer,
        const size_t iBytesSize,
        const audioFormat_t& Oformat,
        void *pOBuffer,
        const size_t oBytesSize)
{
    int formatedInputDepth = (Iformat.bits / 8) + ((Iformat.bits & 0x7) ? 1 : 0);
    int formatedOutputDepth = (Oformat.bits / 8) + ((Oformat.bits & 0x7) ? 1 : 0);
    int formatedInputSampleSize = formatedInputDepth * Iformat.channels;
    int formatedOutputSampleSize = formatedOutputDepth * Oformat.channels;

    /** Calculate If oBytesSize is big enough */
    size_t samplesInputNum = iBytesSize / formatedInputSampleSize;
    size_t samplesOutputNum = samplesInputNum * Oformat.frequence / Iformat.frequence;
    if (samplesOutputNum * formatedOutputSampleSize > oBytesSize)
        return false;

    /** Ignore channels different */
    if (Iformat.channels != Oformat.channels)
        return false;

    const char *_i = static_cast<const char*>(pIBuffer);
    const char *_ri = _i;
    char *_ro = static_cast<char*>(pOBuffer);
    char *_o = _ro;

    size_t mainCnt = samplesInputNum > samplesOutputNum ?
        samplesInputNum : samplesOutputNum;
    bool isExpand = samplesInputNum <= samplesOutputNum;

    /** When in this loop, samplesInputNum may not equal samplesOutputNum */
    for (size_t cnt = 1; cnt <= mainCnt; cnt++)
    {
        /** Loop in channels */
        for (size_t ch = 0; ch < Iformat.channels; ch++)
        {
            /** Bits convert */
            /** This code considered little edition */
            memcpy(_o + formatedOutputSampleSize * ch,
                   _i + formatedInputSampleSize * (ch + 1) - formatedOutputSampleSize,
                   static_cast<size_t>(formatedOutputSampleSize));
        }
        /** Shift ptr contains frequence convert function */
        if (!isExpand) {
            _i += formatedInputSampleSize;
            _o = _ro + formatedOutputSampleSize * (cnt * Oformat.frequence / Iformat.frequence);
        } else {
            _i = _ri + formatedInputSampleSize * (cnt * Iformat.frequence / Oformat.frequence);
            _o += formatedOutputSampleSize;
        }
    }

    return true;
}

bool readWavFormat(wavFormat_t& format, const void* ptr)
{
    /** run id check */
    const char* pChunkID = static_cast<const char*>(ptr);
    const char* pSubChunk1ID = static_cast<const char*>(ptr) + 12;
    const char* pSubChunk2ID = static_cast<const char*>(ptr) + 36;
    const char* pFormat = static_cast<const char*>(ptr) + 8;
    const uint16_t* pAudioFormat = static_cast<const uint16_t*>(ptr) + 10;

    if (strncmp(pChunkID, "RIFF", 4))
        return false;
    if (strncmp(pSubChunk1ID, "fmt ", 4))
        return false;
    if (strncmp(pSubChunk2ID, "data", 4))
        return false;
    if (strncmp(pFormat, "WAVE", 4))
        return false;
    if (*pAudioFormat != 1)
        return false;

    /** Get information */
    const uint16_t *pNumChannels = static_cast<const uint16_t*>(ptr) + 11;
    const uint32_t *pSampleRate = static_cast<const uint32_t*>(ptr) + 6;
    const uint16_t *pBitsPerSample = static_cast<const uint16_t*>(ptr) + 17;
    const uint32_t *pSize = static_cast<const uint32_t*>(ptr) + 10;

    format.channels = *pNumChannels;
    format.frequence = *pSampleRate;
    format.bits = *pBitsPerSample;
    format.totalSize = *pSize;
    return true;
}

int wavTime2Size(const audioFormat_t& format, int ms)
{
    int formatedDepth = format.bits / 8 + ((format.bits & 7) ? 1 : 0);
    int oneSecSize = formatedDepth * format.frequence * format.channels;

    return oneSecSize * ms / 1000;
}

int wavSize2Time( const audioFormat_t& format, int size)
{
    int formatedDepth = format.bits / 8 + ((format.bits & 7) ? 1 : 0);
    int oneSecSize = formatedDepth * format.frequence * format.channels;

    return size * 1000 / oneSecSize;
}

