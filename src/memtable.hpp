#pragma once

#include "skiplist.hpp"

class MemTable
{
    friend class SSTable;
    // 2MB
    static const u32 MAX_SIZE = 1 << 21;

private:
    SkipList<u64, string> skip_list;
    u32 current_size = 0;

public:
    // DEBUG function
    u32 byteSize() { return current_size; }

public:
    void clear()
    {
        current_size = 0;
        skip_list.clear();
    }

    u32 size() { return skip_list.size(); }
    bool empty() { return size() == 0; }

    bool insert(const u64 &key, const string &value)
    {
        auto result = at(key);
        // (key: u64) + (offset: u32) + (value: string)
        u32 entry_size = !result.first
                             ? 12 + value.length()
                             : value.length() - result.second.length();
        if (current_size + entry_size > MAX_SIZE)
            return false; // overflow

        current_size += entry_size;
        skip_list.insert(key, value);
        return true;
    }

    pair<bool, string> at(const u64 &key)
    {
        return skip_list.at(key);
    }
};