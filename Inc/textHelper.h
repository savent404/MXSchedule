#pragma once

#include <stdint.h>
#include <string.h>
#include <string>

#include "color.hpp"
#include "re.h"

using namespace std;

/**
 * @brief getPositionInArray search stirng in a list
 * @param[in] list
 * @param[in] value
 * @param[in] num
 */
int getArrayIndex(
    const string* list,
    const char* value,
    const int listNum);

/**
 * @brief ripe invialid words
 * @param[in] input
 * @param[out] output
 * @note It's a pre-operation for math key-value
 */
void ripWords(const char* input, char* output);

/**
 * @brief matchBankn regex for SETTING.txt: BANK{%d}
 * @param[in] input
 * @param[out] bankIndex
 * @return if matched
 */
bool matchBankn(const char* input, int& bankIndex);

/**
 * @brief matchCFGn regex for match CFG{%d}
 * @param[in] input
 * @param[out] configIndex
 * @return if matched
 */
bool matchCFGn(const char* input, int& configIndex);

/**
 * @name regex functions
 * @param[in] input
 * @param[out] key
 * @param[out] value
 * @return if get key-value is ok
 * @{
 */
bool matchKeyValue(const char* input, std::string& key, int& v);
bool matchKeyValue(const char* input, std::string& key, float& v);
bool matchKeyValue(const char* input, std::string& key, RGB& v);

/** @} */
