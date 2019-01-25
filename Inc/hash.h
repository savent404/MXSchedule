#ifndef MXCORE_HASH_H
#define MXCORE_HASH_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


/**
 * @code
 * int main()
 * {
 *   Hash a(128, sizeof(int);
 *   int data = 10;
 *
 *   a.insert("Hello", &data);
 *   a.insert("World", &data);
 *
 *   int ans;
 *
 *   a.getData("Hello", &ans);
 *   assert(ans == data);
 *   a.getData("World", &ans);
 *   assert(ans == data);
 *
 *
 *   a.remove("Hello");
 *   assert(a.isHas("Hello") == false);
 *
 *   a.clearDeleted();
 *   return 0;
 * }
 * @endcode
 */
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
public:
    Hash(int maximum, size_t elementSize);
    ~Hash();

    bool insert(const char* key, const void* data);
    bool remove(const char* key);
    bool isHas(const char* key);
    bool getData(const char* key, void* data);
    bool setData(const char* key, const void* data);

    /**
     * @brief clear all 'deleted' flag nodes
     * @warning  thread not safe due to updating Hash::arr
     */
    void clearDleted();

protected:
    int findPosition(const char* key);
    __always_inline static void allocElement(element_t* element, const char* name, const void* data, size_t size);
    __always_inline static void freeElement(element_t* element);
    __always_inline static unsigned H1(const char* key);
    __always_inline static unsigned H2(const char* key, unsigned i);
};
#endif //MXCORE_HASH_H
