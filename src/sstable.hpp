#pragma once

#include "common.h"
#include "utils.h"
#include "bloomfilter.hpp"
#include "memtable.hpp"

class SSTable
{
    friend class SSTableLevel;

private:
    u32 level;
    // >>>>> stored in disk
    // NOTE: file_name = "{timestamp}_{min_key}.sst"
    // >>> cached in memory
    // meta-data
    u64 timestamp;
    u64 n_key;
    u64 min_key, max_key;

    // filter
    BloomFilter filter;

    // index section
    //  <key, offset>
    vector<pair<u64, u32>> indices;
    // <<< cached in memory

    // data section
    // vector<string> values;

    // <<<<< stored in disk
    u32 file_size;

private:
    string filePath() const
    {
        return DATA_DIR + "/" +
               "level-" + std::to_string(level) + "/" +
               std::to_string(timestamp) + "_" +
               std::to_string(min_key) + ".sst";
    }

    void saveCached(std::ofstream &fout)
    {
        fout.write(reinterpret_cast<char *>(&timestamp), sizeof(timestamp));
        fout.write(reinterpret_cast<char *>(&n_key), sizeof(n_key));
        fout.write(reinterpret_cast<char *>(&min_key), sizeof(min_key));
        fout.write(reinterpret_cast<char *>(&max_key), sizeof(max_key));
        filter.save(fout);
        for (auto &index : indices)
        {
            fout.write(reinterpret_cast<char *>(&index.first), sizeof(index.first));
            fout.write(reinterpret_cast<char *>(&index.second), sizeof(index.second));
        }
    }

    string loadValue(u32 offset, u32 n_byte) const
    {
        std::ifstream fin(filePath(), std::ios::binary);
        printError(!fin.is_open(), "can't open " + filePath());
        string value(n_byte, '\0');
        fin.seekg(offset);
        fin.read(&value[0], n_byte);
        fin.close();
        return value;
    }

public:
    SSTable() = delete;

    SSTable(u32 level, const string &file_path)
    {
        this->level = level;
        std::ifstream fin(file_path, std::ios::binary);
        printError(!fin.is_open(), "can't open " + file_path);
        fin.read(reinterpret_cast<char *>(&timestamp), sizeof(timestamp));
        fin.read(reinterpret_cast<char *>(&n_key), sizeof(n_key));
        fin.read(reinterpret_cast<char *>(&min_key), sizeof(min_key));
        fin.read(reinterpret_cast<char *>(&max_key), sizeof(max_key));
        filter.load(fin);
        indices.resize(n_key);
        for (auto &index : indices)
        {
            fin.read(reinterpret_cast<char *>(&index.first), sizeof(index.first));
            fin.read(reinterpret_cast<char *>(&index.second), sizeof(index.second));
        }
        fin.seekg(0, std::ios::end);
        file_size = fin.tellg();
        fin.close();
    }

    template <typename InputIt>
    SSTable(InputIt first, InputIt last, u32 level, u64 timestamp)
    {
        this->level = level;
        this->timestamp = timestamp;
        n_key = last - first;

        u32 offset = sizeof(u64) * 4 + (BloomFilter::MAX_SIZE >> 3) +
                     (sizeof(u64) + sizeof(u32)) * n_key;
        // InputIt: pair<u64, string>
        for (auto it = first; it != last; ++it)
        {
            filter.insert(it->first);
            indices.emplace_back(it->first, offset);
            offset += it->second.length();
        }
        min_key = indices.front().first;
        max_key = indices.back().first;

        std::ofstream fout(filePath(), std::ios::binary);
        printError(!fout.is_open(), "can't open " + filePath());
        saveCached(fout);
        for (auto it = first; it != last; ++it)
            fout.write(it->second.c_str(), it->second.length());
        file_size = fout.tellp();
        fout.close();
    }

    SSTable(const MemTable &mem_table, u64 timestamp)
    {
        level = 0;
        this->timestamp = timestamp;
        auto storage =
            mem_table.skip_list.toVector();
        n_key = storage.size();
        min_key = storage.front().first;
        max_key = storage.back().first;

        u32 offset = sizeof(u64) * 4 + (BloomFilter::MAX_SIZE >> 3) +
                     (sizeof(u64) + sizeof(u32)) * n_key;
        indices.reserve(n_key);
        for (const auto &entry : storage)
        {
            filter.insert(entry.first);
            indices.emplace_back(entry.first, offset);
            offset += entry.second.length();
        }

        std::ofstream fout(filePath(), std::ios::binary);
        printError(!fout.is_open(), "can't open " + filePath());
        saveCached(fout);
        for (const auto &entry : storage)
            fout.write(entry.second.c_str(), entry.second.length());
        file_size = fout.tellp();
        fout.close();
    }

    void removeTable()
    {
        utils::rmfile(filePath().c_str());
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

        u32 offset = it->second;
        u32 n_byte = ++it == indices.end()
                         ? file_size - offset
                         : it->second - offset;
        return make_pair(true, loadValue(offset, n_byte));
    }

    vector<pair<u64, string>> toVector()
    {
        u32 value_offset = indices.front().second;
        string str = loadValue(
            value_offset, file_size - value_offset);

        vector<pair<u64, string>> ret;
        for (auto it = indices.begin(); it != indices.end();)
        {
            u64 key = it->first;
            u32 offset = it->second;
            u32 n_byte = ++it == indices.end()
                             ? file_size - offset
                             : it->second - offset;
            ret.emplace_back(std::move(key),
                             str.substr(offset - value_offset, n_byte));
        }
        return ret;
    }
};