#include "textHelper.h"

bool matchKeyValue(const char* input, std::__cxx11::string& key, int& v)
{

    char nameBuffer[32];
    int vv;
    if (re_match("\\w+=\\d+[^,.]*$", input) == -1)
        return false;
    sscanf(input, "%[^=]S", &nameBuffer[0]);
    sscanf(input, "%*[^=]=%d", &vv);
    v = vv;
    key = nameBuffer;
    return true;
}

int getArrayIndex(const string* list, const char* value, const int listNum)
{
    int i = 0;
    const string* ptr = list;

    for (; i < listNum; i++, ptr++) {
        if (!strcasecmp(ptr->c_str(), value))
            return i;
    }
    return -1;
}

void ripWords(const char* input, char* output)
{
    const char* cptr = input;
    char* ptr = output;
    while (*cptr != '\0') {
        if (*cptr == '/') {
            break;
        } else if (*cptr != ' ' && *cptr != '\t' && *cptr != '\n') {
            *ptr++ = *cptr;
        }
        cptr += 1;
    }
    *ptr = '\0';
}

bool matchBankn(const char* input, int& bankIndex)
{
    if (strncasecmp(input, "BANK", 4))
        return false;
    if (re_match("\\w+\\d+", input) == -1)
        return false;
    sscanf(input, "BANK%d", &bankIndex);
    return true;
}

bool matchCFGn(const char* input, int& configIndex)
{
    if (re_match("CFG\\d+:", input) == -1)
        return false;
    sscanf(input, "CFG%d", &configIndex);
    return true;
}

bool matchKeyValue(const char* input, string& key, float& v)
{

    char nameBuffer[32];
    float vv;
    if (re_match("\\w+=\\d+.\\d+[^,]*$", input) == -1)
        return false;
    sscanf(input, "%[^=]s", nameBuffer);
    sscanf(input, "%*[^=]=%f", &vv);
    v = vv;
    key = nameBuffer;
    return true;
}

bool matchKeyValue(const char* input, string& key, RGB& v)
{
    char nameBuffer[32];
    int vBuffer[4];

    if (re_match("\\w+=\\d+,\\d+,\\d+,\\d+", input) == -1) {
        return false;
    }

    sscanf(input, "%[^=]s", nameBuffer);
    sscanf(input, "%*[^=]=%d", vBuffer);
    sscanf(input, "%*[^=]=%*d,%d", vBuffer + 1);
    sscanf(input, "%*[^=]=%*d,%*d,%d", vBuffer + 2);
    sscanf(input, "%*[^=]=%*d,%*d,%*d,%d", vBuffer + 3);
    v.R = uint8_t(vBuffer[0]);
    v.G = uint8_t(vBuffer[1]);
    v.B = uint8_t(vBuffer[2]);
    v.W = 255;
//    v.W = uint8_t(vBuffer[3]);
    key = nameBuffer;
    return true;
}
