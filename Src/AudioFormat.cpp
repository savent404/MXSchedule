#include "AudioFormat.h"

bool convertWavFormat(
        const wavFormat_t& Iformat,
        const void* pIBuffer,
        const size_t iBytesSize,
        const wavFormat_t& Oformat,
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
    bool isExpand = samplesInputNum > samplesOutputNum ?
        false : true;

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
                   formatedOutputSampleSize);
        }
        /** Shift ptr contains frequence convert function */
        if (isExpand == false) {
            _i += formatedInputSampleSize;
            _o = _ro + formatedOutputSampleSize * (cnt * Oformat.frequence / Iformat.frequence);
        } else {
            _i = _ri + formatedInputSampleSize * (cnt * Iformat.frequence / Oformat.frequence);
            _o += formatedOutputSampleSize;
        }
    }

    return true;
}
