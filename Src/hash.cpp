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
        return false;
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

bool Hash::isHas(const char *key) const
{
    int addr = findPosition(key);
    if (addr < 0)
        return false;
    return arr[addr].flag != Hash::element_t::full ? false : true;
}

bool Hash::getData(const char *key, void *data) const
{
    int addr = findPosition(key);
    if (addr < 0 || arr[addr].flag != Hash::element_t::full)
        return false;
    if (!arr[addr].data)
        return false;
    memcpy(data, arr[addr].data, size);
    return true;
}

bool Hash::setData(const char *key, const void *data)
{
    int addr = findPosition(key);
    if (addr < 0 || arr[addr].flag != Hash::element_t::full)
        return false;
    if (!arr[addr].data)
        return false;
    memcpy(arr[addr].data, data, size);
    return true;
}

void Hash::clearDleted()
{
    Hash::element_t *new_arr = new element_t[num];
    Hash::element_t *old_arr = arr;
    arr = new_arr;
    for (int i = 0; i < num; i++)
    {
        new_arr[i].flag = element_t::empty;
        new_arr[i].key = nullptr;
        new_arr[i].data = nullptr;
    }
    arr = new_arr;

    // insert all node into new arr
    for (int i = 0; i < num; i++)
    {
        if (old_arr[i].flag == Hash::element_t::full)
        {
            insert(old_arr[i].key, old_arr[i].data);
        }
    }

    // clear old_arr
    for (int i = 0; i < num; i++)
    {
        freeElement(old_arr + i);
    }
    delete[] old_arr;
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

int Hash::findPosition(const char *key) const
{
    unsigned addr = H1(key);
    unsigned cnt = 0;

    while (arr[addr].flag == Hash::element_t::full || arr[addr].flag == Hash::element_t::deleted)
    {
        if (arr[addr].key && strcmp(arr[addr].key, key) == 0)
            break;

        if (++cnt > num)
            return -1;
        addr = static_cast<unsigned>(H1(key) + H2(key, cnt)) % num;
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
    unsigned seed = 131;
    unsigned hash = 0;
    while (*key)
    {
        hash = hash * seed + (*key++);
    }
    return (hash * i) & 0x7FFFFFFF;
}
