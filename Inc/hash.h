#ifndef MXCORE_HASH_H
#define MXCORE_HASH_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG_HASH
#define DEBUG_HASH 0
#endif

class Hash
{
    typedef struct element_t
    {
        char* key;
        void* data;
        enum { empty, full, deleted } flag;
    } element_t;

    element_t *arr;
    int num;
    size_t size;
#if DEBUG_HASH
    size_t total_find;
    size_t total_refind;
#endif
public:
    Hash(int maximum, size_t elementSize);
    ~Hash();

    bool insert(const char* key, const void* data);
    bool remove(const char* key);
    bool isHas(const char* key);
    bool getData(const char* key, void* data);
    bool setData(const char* key, const void* data);

#if DEBUG_HASH
    size_t getTotalSearch() { return total_find; }
    size_t getTotalResearch() { return total_refind; }
#endif
protected:
    int findPosition(const char* key);
    static void allocElement(element_t* element, const char* name, const void* data, size_t size);
    static void freeElement(element_t* element);
    static unsigned H1(const char* key);
    static unsigned H2(const char* key, unsigned i);
};
#endif //MXCORE_HASH_H
