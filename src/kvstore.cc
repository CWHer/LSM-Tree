#include "kvstore.h"

KVStore::KVStore(const std::string &dir) : KVStoreAPI(dir)
{
    timestamp = 0;
    if (!utils::dirExists(DATA_DIR))
        utils::mkdir(DATA_DIR.c_str());

    if (!utils::scanDir(DATA_DIR).empty())
        timestamp = sstable_level.load() + 1;
}

KVStore::~KVStore()
{
    // store mem_table in disk
    if (!mem_table.empty())
        sstable_level.addTable(
            std::move(SSTable(mem_table, timestamp++)));
    mem_table.clear();
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
    if (!mem_table.insert(key, s))
    {
        // overflow
        sstable_level.addTable(
            std::move(SSTable(mem_table, timestamp++)));
        mem_table.clear();
        mem_table.insert(key, s);
    }
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
    auto result = mem_table.at(key);
    string value = std::move(result.second);
    if (!result.first)
        value = sstable_level.find(key);
    return value != DELETED_TOKEN ? value : EMPTY_TOKEN;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
    if (get(key) == EMPTY_TOKEN)
        return false;

    put(key, DELETED_TOKEN);
    return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
    mem_table.clear();
    sstable_level.clear();
}
