#pragma once

#include "sstable.hpp"

// NOTE: store different level of SSTable
class SSTableLevel
{
private:
    u32 n_level;
    // n_table <= 2 ^ (n + 1)
    vector<list<SSTable>> levels;

    void mkdir()
    {
        string dir_path = DATA_DIR + "/level-" +
                          std::to_string(n_level - 1);
        if (!utils::dirExists(dir_path))
            utils::mkdir((dir_path.c_str()));
    }

    bool overlap(const SSTable &lhs, const SSTable &rhs)
    {
        return !(lhs.max_key < rhs.min_key || rhs.max_key < lhs.min_key);
    }

    void compactTable(u32 current, list<SSTable> &tables)
    {
        // gather into a map
        map<u64, string> entries;
        // insert() skips duplicate, insert the latest first
        for (auto &table : tables)
        {
            auto arr = table.toVector();
            table.removeTable();
            entries.insert(arr.begin(), arr.end());
        }

        auto &current_level = levels[current];
        for (auto it = current_level.begin();
             it != current_level.end();)
        {
            auto last = it++;
            for (auto &table : tables)
                if (overlap(*last, table))
                {
                    auto arr = last->toVector();
                    last->removeTable();
                    entries.insert(arr.begin(), arr.end());
                    current_level.erase(last);
                    break;
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
        u64 timestamp = tables.front().timestamp;
        vector<pair<u64, string>> storage(
            entries.begin(), entries.end());
        static const u32 MAX_SIZE = 1 << 21; // 2MB
        u32 current_size = 0;
        auto last = storage.begin();
        for (auto it = storage.begin(); it != storage.end(); ++it)
        {
            // (key: u64) + (offset: u32) + (value: string)
            u32 entry_size = 12 + it->second.length();
            if (current_size + entry_size > MAX_SIZE)
            {
                current_level.emplace_front(
                    SSTable(last, it, current, timestamp));
                last = it, current_size = 0;
            }
            current_size += entry_size;
        }
        if (last != storage.end())
            current_level.emplace_front(
                SSTable(last, storage.end(), current, timestamp));
    }

public:
    SSTableLevel()
    {
        n_level = 1, mkdir();
        levels.emplace_back();
    }

    u64 load()
    {
        u64 current_timestamp = 0;
        levels.clear();
        n_level = utils::scanDir(DATA_DIR).size();
        levels.resize(n_level);
        for (u32 i = 0; i < n_level; ++i)
        {
            string dir_path = DATA_DIR + "/level-" +
                              std::to_string(i);
            auto files = utils::scanDir(dir_path);
            vector<pair<u64, string>> tables;
            tables.reserve(files.size());
            for (const auto &file : files)
            {
                u64 timestamp = std::stoi(
                    file.substr(0, file.find('_')));
                current_timestamp = std::max(
                    current_timestamp, timestamp);
                tables.emplace_back(
                    std::move(timestamp), dir_path + "/" + file);
            }
            std::sort(tables.begin(), tables.end(),
                      [](const auto &lhs, const auto &rhs)
                      {
                          return lhs.first > rhs.first;
                      });
            for (const auto &entry : tables)
                levels[i].push_back(SSTable(i, entry.second));
        }
        return current_timestamp;
    }

    void clear()
    {
        for (u32 i = 0; i < n_level; ++i)
        {
            for (auto &table : levels[i])
                table.removeTable();
            string dir_path = DATA_DIR + "/level-" +
                              std::to_string(i);
            utils::rmdir(dir_path.c_str());
        }
        levels.clear();

        n_level = 1, mkdir();
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
                n_level++, mkdir(), levels.emplace_back();

            u32 n_table = levels[current].size() -
                          (current == 0 ? 0 : MAX_SIZE(current));
            list<SSTable> tables;
            while (n_table--)
            {
                tables.push_front(std::move(levels[current].back()));
                levels[current].pop_back();
            }

            compactTable(current + 1, tables);
            current++;
        }
    }
};
