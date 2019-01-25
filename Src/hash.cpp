#include "hash.h"

Hash::Hash(int maximum, size_t elementSize)
{
    num = maximum;
    size = elementSize;
    arr = new element_t[num];
    for (int i = 0; i < num; i++)
    {
        arr[i].flag = element_t::empty;
        arr[i].key = nullptr;
        arr[i].data = nullptr;
    }
#if DEBUG_HASH
    total_find = 0;
    total_refind = 0;
#endif
}

Hash::~Hash()
{
    for (int i = 0; i < num; i++)
    {
        freeElement(arr + i);
    }
    delete[] arr;
}

bool Hash::insert(const char *key, const void *data) {
    int addr = findPosition(key);
    if (addr < 0)
        return false;

    if (arr[addr].flag != Hash::element_t::full)
    {
        allocElement(arr + addr, key, data, size);
    }
    else
    {
        memcpy(arr[addr].data, data, size);
    }
    return true;
}

bool Hash::remove(const char *key)
{
    int addr = findPosition(key);
    if (addr < 0)
        return true;
    if (arr[addr].flag == Hash::element_t::full)
    {
        freeElement(arr + addr);
    }
    return true;
}

bool Hash::isHas(const char *key)
{
    int addr = findPosition(key);
    if (addr < 0)
        return false;
    return arr[addr].flag != Hash::element_t::full ? false : true;
}

bool Hash::getData(const char *key, void *data)
{
    int addr = findPosition(key);
    if (addr < 0 || arr[addr].flag != Hash::element_t::full)
        return false;
    memcpy(data, arr[addr].data, size);
    return true;
}

bool Hash::setData(const char *key, const void *data)
{
    int addr = findPosition(key);
    if (addr < 0 || arr[addr].flag != Hash::element_t::full)
        return false;
    memcpy(arr[addr].data, data, size);
    return true;
}

void Hash::allocElement(Hash::element_t *element, const char *name, const void *data, size_t size)
{
    if (element->flag == Hash::element_t::full)
        return;
    freeElement(element);
    element->key = static_cast<char*>(malloc(strlen(name) + 1));
    element->data = malloc(size);

    strcpy(element->key, name);
    memcpy(element->data, data, size);
    element->flag = Hash::element_t::full;
}

void Hash::freeElement(Hash::element_t *element)
{
    if (element->flag == Hash::element_t::empty)
        return;
    element->flag = Hash::element_t::deleted;
    if (element->key)
    {
        free(element->key);
        element->key = nullptr;
    }
    if (element->data)
    {
        free(element->data);
        element->data = nullptr;
    }
}

int Hash::findPosition(const char *key)
{
    unsigned addr = H1(key);
    unsigned cnt = 0;

#if DEBUG_HASH
    this->total_find++;
#endif

    while (arr[addr].flag == element_t::full)
    {
        if (strcasecmp(arr[addr].key, key) == 0)
            break;

        if (++cnt > num)
            return -1;
        addr = static_cast<unsigned>(H1(key) + H2(key, cnt)) % num;
#if DEBUG_HASH
        this->total_refind++;
#endif
    }
    return static_cast<int>(addr);
}

unsigned Hash::H1(const char* key)
{
    unsigned hash = 0;
    unsigned  x = 0;
    char c;
    do {
        c = *key++;
        if ((x = hash & 0xF0000000) != 0)
        {
            hash ^= (x >> 24);
            hash &= ~x;
        }
    } while (c);
    return hash & 0x7FFFFFFF;
}

unsigned Hash::H2(const char* key, unsigned i)
{
    return i*H1(key) + i;
}
