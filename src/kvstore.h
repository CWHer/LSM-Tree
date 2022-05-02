#pragma once

#include "common.h"
#include "kvstore_api.h"
#include "memtable.hpp"
#include "sstable_level.hpp"

class KVStore : public KVStoreAPI
{
    // You can add your implementation here
private:
    u32 timestamp;
    MemTable mem_table;
    SSTableLevel sstable_level;

public:
    KVStore(const std::string &dir);

    ~KVStore();

    void put(uint64_t key, const std::string &s) override;

    std::string get(uint64_t key) override;

    bool del(uint64_t key) override;

    void reset() override;
};
