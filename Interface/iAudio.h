#pragma once

#include "common.h"
#include "iParam.h"
#include <stdint.h>
class iAudio : public iEvent {
public:
    typedef enum event_t {
        start = 0x01,
        end = 0x02,
        track1 = 0x04,
        track2 = 0x08,
        track3 = 0x10,
    } event_t;

    typedef enum mode_t {
        oneShout = 0,
        loop = 1,
    } mode_t;

    typedef struct track_t {
        bool busy;
        std::string path;
        mode_t mode;
        size_t audioSize;
    } track_t;

    typedef int trackId_t;
protected:
    const iParam* parameter;
    track_t track[3];
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
//        size_t n = sizeof(track) / sizeof(track[0]);
//        for (size_t i = 0; i < n ;i++)
//            abort(i);
    }

    trackId_t play(triggerID_t id);
    trackId_t mainTrack(bool enable);
    virtual trackId_t _play(const char* wavPath, mode_t mode = oneShout) =  0;

    virtual bool abort(trackId_t id) = 0;
};
