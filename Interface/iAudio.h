#pragma once

#include "common.h"
#include "iParam.h"
#include <stdint.h>

/**
 * @brief 音频模块
 * @details 支持最高3个track同时播放
 */
class iAudio : public iEvent {
public:
    /**
     * @brief audio event
     * @details
     * - start start event
     * - end   end/abort event
     * - track1 identify it's track_1, mixed with start or end
     * - track2 identify it's track_2, mixed with start or end
     * - track3 identify it's track_3, mixed with start or end
     * @note
     * 发送事件时需要start/end与trackID组合使用:
     * @code
     * sendEvent(track1 | start);
     * @endcode
     */
    typedef enum event_t {
        start = 0x01,
        end = 0x02,
        track1 = 0x04,
        track2 = 0x08,
        track3 = 0x10,
    } event_t;

    /**
     * @brief defin audio play-mode
     * @details
     * - oneShout play once then auto-stop
     * - play in loop, only call abort can stop it.
     */
    typedef enum mode_t {
        oneShout = 0,
        loop = 1,
    } mode_t;

    /**
     * @brief track's main message
     * @note  more message can defined in sub-class
     * @details
     * - busy identify if this track is playing
     * - path file full path
     * - mode play mode
     * - T    play totoal time(ms)
     */
    typedef struct track_t {
        bool busy;
        std::string path;
        mode_t mode;
        uint32_t T;
    } track_t;

    typedef int trackId_t;

protected:
    /** @brief should get parameter from this instance */
    const iParam* parameter;
    /** @brief MX support 3 track */
    track_t track[3];
    /** @brief storage hum.wav in which track */
    trackId_t mainTrackId;

public:
    iAudio(const iParam* p)
        : iEvent(EVENT_MODULE_ID_AUDIO)
        , parameter(p)
    {
        setEventMask(end);
        size_t n = sizeof(track) / sizeof(track[0]);
        for (size_t i = 0; i < n; i++)
            track[i].busy = false;
        mainTrackId = -1;
    }

    virtual ~iAudio()
    {
    }

    /**
     * @brief play
     * @param id
     * @param comboIndex if triggerd a combo,should give index
     * @note  this mothod would call iAudio::_play
     * @return trigger's track id
     */
    trackId_t play(triggerID_t id, int comboIndex = -1);

    /**
     * @brief mainTrack enable/disable background audio
     * @param enable
     * @return mainTrack's track id
     */
    trackId_t mainTrack(bool enable);

    /**
     * @brief _play
     * @param wavPath
     * @param mode
     * @return audio's track id
     */
    virtual trackId_t _play(const char* wavPath, mode_t mode = oneShout) = 0;

    /**
     * @brief abort
     * @param id
     * @return
     */
    virtual bool abort(trackId_t id) = 0;

    /**
     * @brief getTrackDuration get audio last time(ms)
     * @param id
     * @return last time(ms)
     */
    virtual uint32_t getTrackDuration(trackId_t id) = 0;
};
