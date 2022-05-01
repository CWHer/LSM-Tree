#pragma once

#include "common.h"
#include "bloomfilter.hpp"
#include "memtable.hpp"

class SSTable
{
private:
    // >>>>> stored in disk
    // >>> cached in memory
    // meta-data
    u64 timestamp;
    u64 n_key;
    u64 min_key, max_key;

    // filter
    BloomFilter filter;

    // index section
    //  <key, offset>
    // TODO: change offset
    vector<pair<u64, u32>> indices;
    // <<< cached in memory

    // data section
    vector<string> values;

    // <<<<< stored in disk
public:
    SSTable() = delete;

    SSTable(const MemTable &mem_table, u64 timestamp)
    {
        this->timestamp = timestamp;
        auto storage =
            mem_table.skip_list.toVector();
        n_key = storage.size();
        min_key = storage.front().first;
        max_key = storage.back().first;

        u32 offset = 0;
        indices.reserve(n_key);
        for (const auto &entry : storage)
        {
            filter.insert(entry.first);
            indices.emplace_back(entry.first, offset);
            // offset += entry.second.length();
            offset++;
        }

        // TODO: remove this
        for (const auto &entry : storage)
            values.push_back(std::move(entry.second));
    }

    void load()
    {
        // TODO
    }

    void save()
    {
        // TODO
    }

    void removeTable()
    {
        // TODO
    }

    pair<bool, string> find(const u64 &key)
    {
        if (!filter.query(key))
            return make_pair(false, "");

        auto it = std::lower_bound(
            indices.begin(), indices.end(), key,
            [](const auto &lhs, const u64 &k)
            { return lhs.first < k; });
        if (it == indices.end() ||
            it->first != key)
            return make_pair(false, "");

        // TODO: load values
        return make_pair(true, values[it->second]);
    }
};