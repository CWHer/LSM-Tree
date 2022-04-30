#ifndef __BLOOMFILTER_HPP__
#define __BLOOMFILTER_HPP__

#include "common.h"
#include "murmurhash3.h"

class BloomFilter
{
    // 10KB
    static const i32 MAX_SIZE = 10240;

private:
    bitset<MAX_SIZE> b;

public:
    void clear() { b.reset(); }

    void insert(i64 key)
    {
        u32 hash[4];
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);

        for (i32 i = 0; i < 4; ++i)
            b.set(hash[i] % MAX_SIZE);
    }

    // WARN: false positive
    bool query(i64 key)
    {
        u32 hash[4];
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);

        bool result = true;
        for (i32 i = 0; i < 4; ++i)
            result &= b[hash[0] % MAX_SIZE];
        return result;
    }
};

#endif