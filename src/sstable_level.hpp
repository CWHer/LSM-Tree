#pragma once

#include "sstable.hpp"

// NOTE: store different level of SSTable
class SSTableLevel
{
private:
    u32 n_level;
    // n_table <= 2 ^ (n + 1)
    vector<list<SSTable>> levels;

    bool overlap(const SSTable &lhs, const SSTable &rhs)
    {
        return !(lhs.max_key < rhs.min_key || rhs.max_key < lhs.min_key);
    }

    void compactTable(u32 current, SSTable &&table)
    {
        // gather into a map
        map<u64, string> entries;
        auto arr = table.toVector();
        // skip duplicate, insert the latest first
        entries.insert(arr.begin(), arr.end());

        auto &current_level = levels[current];
        for (auto it = current_level.begin();
             it != current_level.end();)
        {
            auto last = it++;
            if (overlap(*last, table))
            {
                auto arr = last->toVector();
                entries.insert(arr.begin(), arr.end());
                current_level.erase(last);
            }
        }

        // remove DELETED in last level
        if (current == n_level - 1)
            for (auto it = entries.begin();
                 it != entries.end();)
            {
                auto last = it++;
                if (last->second == DELETED_TOKEN)
                    entries.erase(last);
            }

        // split into new sstable
        vector<pair<u64, string>> storage(
            entries.begin(), entries.end());
        static const u32 MAX_SIZE = 1 << 21; // 2 MB
        u32 current_size = 0;
        auto last = storage.begin();
        for (auto it = storage.begin(); it != storage.end(); ++it)
        {
            // (key: u64) + (offset: u32) + (value: string)
            u32 entry_size = 12 + it->second.length();
            if (current_size + entry_size > MAX_SIZE)
            {
                current_level.emplace_front(
                    SSTable(last, it, table.timestamp));
                last = it, current_size = 0;
            }
            current_size += entry_size;
        }
        if (last != storage.end())
            current_level.emplace_front(
                SSTable(last, storage.end(), table.timestamp));
    }

public:
    SSTableLevel()
    {
        n_level = 1;
        levels.emplace_back();
    }

    string find(const u64 &key)
    {
        for (u32 i = 0; i < n_level; ++i)
            for (const auto &table : levels[i])
            {
                auto result = table.find(key);
                if (result.first)
                    return result.second;
            }
        return EMPTY_TOKEN;
    }

    void addTable(SSTable &&table)
    {

        levels[0].emplace_front(table);

        static auto MAX_SIZE = [](u32 x) -> u32
        { return 1 << x << 1; };

        // compaction
        //  1. level 0 ---> level 1
        //      all level 0 sstable
        //  2. other levels
        //      only excessive (small timestamp first) sstable
        u32 current = 0;
        while (levels[current].size() >
               MAX_SIZE(current))
        {
            if (current + 1 == n_level)
                n_level++, levels.emplace_back();

            u32 n_table = levels[current].size() -
                          (current == 0 ? 0 : MAX_SIZE(current));
            for (u32 i = 0; i < n_table; ++i)
            {
                compactTable(current + 1,
                             std::move(levels[current].back()));
                levels[current].pop_back();
            }
            current++;
        }
    }
};
