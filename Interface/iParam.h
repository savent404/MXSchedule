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

class iParam : public iEvent {

public:
    /** @brief iParam's event */
    typedef enum {
        inited = 0x01,
        crash = 0x02,
    } event_t;

protected:
    /** @name bank related parameter
     * @{ */
    /** Bank position now */
    int posBank;
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
    std::vector<int> intParam;
    std::vector<float> floatParam;
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
protected:
    /** @} */
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

public:
    iParam(int bank = 0)
        : iEvent(EVENT_MODULE_ID_PARAM)
        , posBank(bank)
        , numBank(0)
    {
        setEventMask(inited | crash);
        // readStaticParameter();
        // switchBank(bank);
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
     * @brief getBankNum
     * @return bank's number
     */
    int getBankNum() const;

    /**
     * @brief getBankPos
     * @return bank's position
     */
    int getBankPos() const;

    /**
     * @brief getBankName
     * @return bank's name
     */
    std::string getBankName() const;

    /**
     * @brief getTriggerNum
     * @param id
     * @return number
     */
    int getTriggerNum(triggerID_t id) const;
};
