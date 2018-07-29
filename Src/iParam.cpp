#include "iParam.h"

using namespace std;

void iParam::setParameterFromLine(const char* line)
{
    /** Use static var can save constructor/des-xx time
     */
    static int aInt;
    static float aFloat;
    static RGB aRGB;

    static char buffer[128];
    static string key;

    ripWords(line, buffer);
    if (matchKeyValue(buffer, key, aInt)) {
        bool isTypeInt = setParameter(key.c_str(), aInt);
        if (isTypeInt == false) {
            float realFloat = aInt;
            setParameter(key.c_str(), realFloat);
        }
    } else if (matchKeyValue(buffer, key, aRGB)) {
        setParameter(key.c_str(), aRGB);
    } else if (matchKeyValue(buffer, key, aFloat)) {
        setParameter(key.c_str(), aFloat);
    }
}

void iParam::setColorParameterFromLine(const char* line)
{
    static int configIndex = 0;
    static char buffer[128];
    static RGB rgb;
    static std::string key;

    ripWords(line, buffer);
    if (matchKeyValue(buffer, key, rgb)) {
        size_t RGBColorNum = sizeof(typeRGBParam) / sizeof(string);
        int pos = getArrayIndex(typeRGBParam, key.c_str(), RGBColorNum);
        if (pos < 0)
            return;
        staticParam.configRGB[configIndex * RGBColorNum + pos] = rgb;
    } else if (matchCFGn(buffer, configIndex)) {
        configIndex -= 1;
    }
}

void iParam::setDefaultParameter()
{
}

bool iParam::getParameter(const char* name, int& v) const
{
    int pos = getArrayIndex(&typeIntParam[0], name, sizeof(typeIntParam) / sizeof(string));
    if (pos < 0) {
        int bankID;
        if (!matchBankn(name, bankID) || bankID > getBankNum()) {
            return false;
        }
        v = staticParam.configRGBIndex[bankID - 1];
        return true;
    }
    v = intParam[pos];
    return true;
}

bool iParam::getParameter(const char* name, float& v) const
{
    int pos = getArrayIndex(&typeFloatParam[0], name, sizeof(typeFloatParam) / sizeof(string));
    if (pos < 0)
        return false;
    v = floatParam[pos];
    return true;
}

bool iParam::getParameter(const char* name, RGB& v) const
{
    size_t configNum = sizeof(typeRGBParam) / sizeof(string);

    int pos = getArrayIndex(typeRGBParam, name, configNum);

    if (pos < 0)
        return false;

    int configIndex = staticParam.configRGBIndex[getBankPos()];

    if (configIndex >= getBankNum()) {
        return false;
    }

    v = staticParam.configRGB[configNum * configIndex + pos];
    return true;
}

bool iParam::setParameter(const char* name, const int& v)
{
    int pos = getArrayIndex(typeIntParam, name, sizeof(typeIntParam) / sizeof(string));

    if (pos < 0) {
        int bankID;
        if (!matchBankn(name, bankID) || bankID > getBankNum()) {
            return false;
        }
        staticParam.configRGBIndex[bankID - 1] = v;
        return true;
    }
    intParam[pos] = v;
    return true;
}

bool iParam::setParameter(const char* name, const float& v)
{
    int pos = getArrayIndex(typeFloatParam, name, sizeof(typeFloatParam) / sizeof(string));

    if (pos < 0)
        return false;
    floatParam[pos] = v;
    return true;
}

bool iParam::setParameter(const char* name, const RGB& v)
{
    size_t configNum = sizeof(typeRGBParam) / sizeof(string);
    int pos = getArrayIndex(typeRGBParam, name, configNum);
    if (pos < 0)
        return false;
    int configIndex = staticParam.configRGBIndex[getBankPos()];
    if (size_t(configIndex) >= staticParam.configRGB.size()) {
        return false;
    }
    staticParam.configRGB[configIndex * configNum + pos] = v;
    return true;
}

bool iParam::switchBank(int pos)
{
    std::string v;

    // Calculate bank pos, bank path
    if (pos >= getBankNum()) {
        pos = getBankNum() - 1;
    }

    if (pos < 0) {
        pos = (getBankPos() + 1) % getBankNum();
    }

    if (!searchFileName(workPath.c_str(), "BANK*", v, pos)) {
        v = "";
    }

    v = workPath + v + "/";
    bankName = v;
    posBank = pos;
    staticParam.posBank = getBankPos();

    // set parameter
    for (size_t i = 0; i < sizeof(triggerName) / sizeof(string); i++) {
        string path = bankName + triggerName[i] + "/";
        triggerNum[i] = searchFileCnt(path.c_str(), "\\w+.WAV");
    }

    setDefaultParameter();

    if (!readConfigFromFile((workPath + "SETTING.txt").c_str())) {
        return false;
    }

    v = getBankName();
    v += "Effect.txt";
    readConfigFromFile(v.c_str());

    return true;
}

int iParam::getBankNum() const
{
    return numBank;
}

int iParam::getBankPos() const
{
    return posBank;
}

string iParam::getBankName() const
{
    return bankName;
}

int iParam::getTriggerNum(triggerID_t id) const
{
    return triggerNum[id];
}
