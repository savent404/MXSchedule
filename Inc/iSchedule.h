#pragma once

#include "common.h"
#include "iAudio.h"
#include "iBlade.h"
#include "iEvent.hpp"
#include "iKey.h"
#include "iParam.h"

typedef struct iDriverList {
    iAudio* audio;
    iParam* param;
    iBlade* blade;
    iKey* key;
} iDriverList;

class iShechdule {
private:
    iDriverList list;

public:
    iShechdule(iDriverList l);
    ~iShechdule();

    void run();
};
