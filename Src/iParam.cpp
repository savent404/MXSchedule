#include "iParam.h"

using namespace std;
bool iParam::initCallback()
{
    numBank = searchFileCnt(workPath.c_str(), "BANK*");
    // insert every single parameter
    int val = 0;
    for (size_t i = 0; i < sizeof(typeIntParam) / sizeof(string); i++)
    {
        intParam.insert(typeIntParam[i].c_str(), &val);
    }
    floatParam.resize(sizeof(typeFloatParam) / sizeof(string));

    // staticParameter
    if (readStaticParameter() && (static_cast<size_t>(getBankNum()) == staticParam.configRGBIndex.size()))
    {
        posBank = staticParam.posBank;
    } else {
        initStaticParameter();
    }

    // init defualt setting
    if (!readConfigFromFile((workPath + "SETTING.txt").c_str())) {
        mDebug(DEBUG_LEVEL_ERROR, "Open SETTING.txt error");
        return false;
    }
    // init ColorConfig
    if (!readColorConfigFromFile((workPath + "COLORMATRIX.txt").c_str())) {
        mDebug(DEBUG_LEVEL_ERROR, "Open COLORMATRIX.txt error");
        return false;
    }
    switchBank(posBank);
    inited = true;
    return true;
}
void iParam::setParameterFromLine(const char* line)
{
    /** Use static var can save constructor/des-xx time
     */
    static int aInt;
    static float aFloat;
    static RGB aRGB;

    char buffer[128];
    char comboSequence[64];
    int comboIndex;
    int comboPriorty;
    static string key;

    ripWords(line, buffer);
    if (matchComboN(buffer, comboIndex, comboSequence, comboPriorty)) {
        int len = static_cast<int>(strlen(comboSequence));
        bool isRight = true;
        combo_t newCombo(len, comboPriorty, comboIndex);
        for (int i = 0; i < len; i++)
        {
            if (comboSequence[i] == 's' || comboSequence[i] == 'S')
                newCombo.sequence[i] = 0;
            else if (comboSequence[i] == 'c' || comboSequence[i] == 'C')
                newCombo.sequence[i] = 1;
            else if (comboSequence[i] == 'b' || comboSequence[i] == 'B')
                newCombo.sequence[i] = 2;
            else {
                isRight = false;
                break;
            }
        }
        if (isRight == true)
        {
            comboParam.push_back(newCombo);
        }
    } else if (matchKeyValue(buffer, key, aInt)) {
        if (!setParameter(key.c_str(), aInt)) {
            if (!setBankColorIndex(key.c_str(), aInt)) {
                setParameter(key.c_str(), static_cast<float>(aInt));
            }
        }
    } else if (matchKeyValue(buffer, key, aRGB)) {
        setParameter(key.c_str(), aRGB);
    }
}

void iParam::setColorParameterFromLine(const char* line)
{
    static int configIndex = 0;
    char buffer[128];
    RGB rgb;
    std::string key; // reduce con/destruct time.

    size_t RGBColorNum = sizeof(typeRGBParam) / sizeof(string);

    ripWords(line, buffer);
    if (matchKeyValue(buffer, key, rgb)) {
        int pos = getArrayIndex(typeRGBParam, key.c_str(), RGBColorNum);
        if (pos < 0)
            return;
        int cood = configIndex * RGBColorNum + pos;
        if (staticParam.configRGB.size() <= cood)
            staticParam.configRGB.resize(cood + 1);
        staticParam.configRGB.at(configIndex * RGBColorNum + pos) = rgb;
    } else if (matchCFGn(buffer, configIndex) && configIndex > 0) {
        configIndex -= 1;
    }
}

void iParam::setDefaultParameter()
{

}

bool iParam::getParameter(const char* name, int& v) const
{
    return intParam.getData(name, &v);
}

bool iParam::getParameter(const char* name, float& v) const
{
    int pos = getArrayIndex(&typeFloatParam[0], name, sizeof(typeFloatParam) / sizeof(string));
    if (pos < 0)
        return false;
    v = floatParam.at(pos);
    return true;
}

bool iParam::getParameter(const char* name, RGB& v) const
{
    size_t configNum = sizeof(typeRGBParam) / sizeof(string);

    int pos = getArrayIndex(typeRGBParam, name, configNum);

    if (pos < 0)
        return false;

    int configIndex = staticParam.configRGBIndex.at((getBankPos() + colorPosBank) % getBankNum());

    if (configIndex >= getBankNum()) {
        return false;
    }

    v = staticParam.configRGB.at(configNum * configIndex + pos);
    return true;
}

bool iParam::setParameter(const char* name, const int& v)
{
    return intParam.setData(name, &v);
}

bool iParam::setParameter(const char* name, const float& v)
{
    int pos = getArrayIndex(typeFloatParam, name, sizeof(typeFloatParam) / sizeof(string));
    if (pos < 0)
        return false;
    floatParam.at(static_cast<size_t>(pos)) = v;
    return true;
}

bool iParam::setParameter(const char* name, const RGB& v)
{
    int configNum = static_cast<int>(sizeof(typeRGBParam) / sizeof(string));
    if (getArrayIndex(typeRGBParam, name, configNum) < 0)
        return false;
    auto pos = static_cast<size_t>((getBankPos() + colorPosBank) % getBankNum());
    int configIndex = staticParam.configRGBIndex.at(pos);
    if (size_t(configIndex) >= staticParam.configRGB.size()) {
        return false;
    }
    staticParam.configRGB.at(configIndex * configNum + pos) = v;
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

    if (!searchFileName(workPath.c_str(), "[Bb][Aa][Nn][Kk]+", v, pos)) {
        v = "";
    }

    bankName = workPath + v + "/";
    posBank = pos;
    staticParam.posBank = getBankPos();

    // set parameter
    for (size_t i = 0; i < sizeof(triggerName) / sizeof(string); i++) {
        string path = bankName + triggerName[i] + "/";
        triggerNum[i] = searchFileCnt(path.c_str(), "\\w+.[Ww][Aa][Vv]");
    }

    // clear all storaged combo parameter
    comboParam.clear();
    setDefaultParameter();

    if (!readConfigFromFile((workPath + "SETTING.txt").c_str())) {
        return false;
    }

    v = getBankName() + "Effect.txt";
    readConfigFromFile(v.c_str());

    return true;
}

