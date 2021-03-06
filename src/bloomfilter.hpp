#pragma once

#include "common.h"
#include "murmurhash3.h"

class BloomFilter
{
public:
    // 10KB
    static const i32 MAX_SIZE = 10 << 10 << 3;

private:
    bitset<MAX_SIZE> b;

public:
    void clear() { b.reset(); }

    void insert(const u64 &key)
    {
        u32 hash[4];
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);

        for (i32 i = 0; i < 4; ++i)
            b.set(hash[i] % MAX_SIZE);
    }

    // WARN: false positive
    bool query(const u64 &key) const
    {
        u32 hash[4];
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);

        bool result = true;
        for (i32 i = 0; i < 4; ++i)
            result &= b[hash[0] % MAX_SIZE];
        return result;
    }

    void load(std::ifstream &fin)
    {
        fin.read(reinterpret_cast<char *>(&b), sizeof(b));
    }

    void save(std::ofstream &fout)
    {
        fout.write(reinterpret_cast<char *>(&b), sizeof(b));
    }
};
