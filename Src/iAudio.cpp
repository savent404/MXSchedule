#include "iAudio.h"

using namespace std;

iAudio::trackId_t iAudio::play(triggerID_t id, int comboIndex)
{
    string filePath;
    string dirPath;
    if (ColorSwitch == id)
    {
        dirPath = parameter->getPrefixPath() + "System/";
        filePath = "ColorSwitch.wav";
    }
    else if (Combo == id)
    {
        dirPath = parameter->getBankName();
        filePath = "combo";
        filePath += comboIndex;
        filePath += ".wav";
    }
    else if (Unknow == id)
    {
        mDebug(DEBUG_LEVEL_ERROR, "Unspport audio trigger");
        return false;
    }
    else
    {
        dirPath = parameter->getBankName() + triggerName[id];
        int nTrigger = parameter->getTriggerNum(id);
        if (nTrigger <= 0)
            return -1;
        if (!parameter->searchFileName(dirPath.c_str(), "\\w+.WAV", filePath, rand() % nTrigger))
        {
            mDebug(DEBUG_LEVEL_ERROR, "Can't find trigger file path."
              "Trigger:%d, Bank:%d",
              triggerName[id],
              parameter->getBankName());
            return false;
        }
    }

    return _play((dirPath + "/" + filePath).c_str(), Lockup == id ? loop : oneShout);
}

iAudio::trackId_t iAudio::mainTrack(bool enable)
{
   if (enable)
   {
       string filePath = parameter->getBankName() + "hum.wav";
       mainTrackId = _play(filePath.c_str(), loop);
   }
   else
   {
       abort(mainTrackId);
       mainTrackId = -1;
   }
}
