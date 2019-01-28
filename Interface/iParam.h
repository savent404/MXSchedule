#pragma once

#include "color.hpp"
#include "common.h"
#include "iEvent.hpp"
#include "re.h"
#include "textHelper.h"
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include "hash.h"
#include "combo.h"

/**
 * @brief 用户参数管理
 */
class iParam : public iEvent {

public:
    /** @brief iParam's event */
    typedef enum {
        crash = 0x01,
    } event_t;

protected:
    /** @name bank related parameter
     * @{ */
    /** Bank position now */
    int posBank;
    int colorPosBank;
    /** Bank's total number */
    int numBank;
    /** Bank's path or name*/
    std::string bankName;
    /** Bank's prefix */
    std::string workPath;
    /** @} */

    /** 
     * @name parameter list, classify by type 
     * @{ */
    Hash intParam;
    std::vector<float> floatParam;
    std::vector<combo_t> comboParam;
    /** @} */

    /**
     * @name trigger related parameter
     * @{ */
    /** storage each trigger's number of wav files */
    int triggerNum[11];
    /** @} */

    /**
     * @name Static Flash parameter
     * @{ */
    /** @brief static parameter, should storaged in flash */
    struct
    {
        int posBank;
        std::vector<RGB> configRGB;
        std::vector<int> configRGBIndex;
    } staticParam;
    /** @} */

public:
    /**
     * @name port func
     * @{ */
    virtual int searchFileCnt(const char* path, const char* regex) const = 0;
    virtual bool searchFileName(const char* path, const char* regex, std::string& out, int pos) const = 0;
    virtual bool readConfigFromFile(const char* filepath) = 0;
    virtual bool readColorConfigFromFile(const char* filepath) = 0;
    virtual bool readStaticParameter() = 0;
    virtual bool writeStaticParameter() = 0;
    virtual void initStaticParameter() {
        size_t rgbNum = sizeof(typeRGBParam) / sizeof(string);
        staticParam.configRGBIndex.reserve(getBankNum());
        staticParam.configRGB.reserve(getBankNum() * rgbNum);
        staticParam.configRGBIndex.resize(getBankNum());
        // posBank = posBank;
    }
    /** @} */
protected:

    /**
     * @name text operation
     * @{ */
    /**
     * @brief setParameterFromLine read param from one line
     * @param line
     */
    void setParameterFromLine(const char* line);
    /**
     * @brief setColorParameterFromLine
     * @param line
     */
    void setColorParameterFromLine(const char* line);
    /** @} */
    /**
     * @brief setDefaultParameter
     */
    void setDefaultParameter();

    inline bool setBankColorIndex(const char* name, const int& v)
    {
        int bank;
        if (!matchBankn(name, bank) || bank > getBankNum() || bank < 1)
            return false;
        staticParam.configRGBIndex[bank - 1] = v;
        return true;
    }

    bool initCallback();

public:
    /**
     * @brief 标记初始化是否完成
     */
    bool inited;

    iParam(int bank = 0)
        : iEvent(EVENT_MODULE_ID_PARAM)
        , posBank(bank)
        , colorPosBank(0)
        , numBank(0)
        , inited(false)
        , intParam(64, sizeof(int))
    {
        setEventMask(crash);
        // In sub-class, u should init attribute: 'workPath'
        // After constructure, call 'initCallback'
        // at the end and check the return val
    }

    virtual ~iParam()
    {
        // writeStaticParameter();
    }

    /**
     * @name getParameter get parameter
     * @details get parameter classified by different types
     * @param[in] name parameter's name, should match type[xx]Param
     * @param[out] v
     * @return if get parameter is ok
     * @{ */
    bool getParameter(const char* name, int& v) const;
    bool getParameter(const char* name, float& v) const;
    bool getParameter(const char* name, RGB& v) const;
    /** @} */

    /**
     * @name setParameter
     * @details set parameter classified by different types
     * @param[in] name
     * @param[out] v
     * @return if set parameter is ok
     */
    bool setParameter(const char* name, const int& v);
    bool setParameter(const char* name, const float& v);
    bool setParameter(const char* name, const RGB& v);
    /** @} */

    /**
     * @brief switchBank
     * @param pos spicific bank pos u choosed, or -1 means next one
     * @return if switch is ok
     */
    bool switchBank(int pos = -1);

    /**
     * @brief colorSwitch更新颜色参数的index
     */
    bool incColorPos() { colorPosBank++; return true; }

    /**
     * @brief 清除ColorSwitch造成的bank颜色不符
     */
    bool resetColorPos() { colorPosBank = 0; return true; }

    /**
     * @brief getBankNum
     * @return bank's number
     */
    int getBankNum() const { return numBank; }

    /**
     * @brief getBankPos
     * @return bank's position
     */
    int getBankPos() const { return posBank; }

    /**
     * @brief getBankName
     * @return bank's name
     */
    std::string getBankName() const { return bankName; }

    /**
     * @brief get Bank's prefix path
     * @return string
     */
    std::string getPrefixPath() const { return workPath; }

    /**
     * @brief getTriggerNum
     * @param id
     * @return number
     */
    int getTriggerNum(triggerID_t id) const { return triggerNum[id]; }

    /**
     * @brief comboList
     * @return combo's array
     * @note It's a const method, and return const array
     */
    const vector<combo_t> *comboList() const { return &comboParam; }
};
