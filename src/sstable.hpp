#pragma once

#include "common.h"
#include "bloomfilter.hpp"
#include "memtable.hpp"

class SSTable
{
    friend class SSTableLevel;

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

    template <typename InputIt>
    SSTable(InputIt first, InputIt last, u64 timestamp)
    {
        this->timestamp = timestamp;

        u32 offset = 0;
        // pair<u64, string>
        for (auto it = first; it != last; ++it)
        {
            filter.insert(it->first);
            indices.emplace_back(it->first, offset);

            // TODO
            offset += 1;
            values.emplace_back(std::move(it->second));
        }

        n_key = indices.size();
        min_key = indices.front().first;
        max_key = indices.back().first;
    }

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
            values.emplace_back(std::move(entry.second));
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

    pair<bool, string> find(const u64 &key) const
    {
        if (!filter.query(key))
            return make_pair(false, EMPTY_TOKEN);

        auto it = std::lower_bound(
            indices.begin(), indices.end(), key,
            [](const auto &lhs, const u64 &k)
            { return lhs.first < k; });
        if (it == indices.end() ||
            it->first != key)
            return make_pair(false, EMPTY_TOKEN);

        // TODO: load values
        return make_pair(true, values[it->second]);
    }

    // WARN: would invalid table
    vector<pair<u64, string>> toVector()
    {
        vector<pair<u64, string>> ret;
        for (auto &index : indices)
            ret.emplace_back(
                std::move(index.first),
                std::move(values[index.second]));
        return ret;
    }
};