//
// Created by savent on 19-1-27.
//

#ifndef QT_MX_COMBO_H
#define QT_MX_COMBO_H

#include <stdint.h>

/**
 * @brief The combo_t struct
 * @details storage Combo's msg: priority, id and sequence
 */
struct combo_t {
    int id;
    int priority;
    int len;

    uint8_t *sequence;

    combo_t(int length, int pri = 1, int playId = 1)
    {
        if (length <= 0)
            sequence = nullptr;
        else {
            sequence = new uint8_t[length];
        }
        len = length;
        id = playId;
        priority = pri;
    }

    combo_t(const combo_t& other) : combo_t(other.len, other.priority, other.id)
    {
    }

    ~combo_t()
    {
        if (sequence)
        {
            delete []sequence;
        }
    }
};
#endif //QT_MX_COMBO_H
