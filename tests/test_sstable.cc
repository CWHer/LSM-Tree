#include "memtable.hpp"
#include "sstable.hpp"

MemTable mem_table;

int main()
{
    printInfo("Test1");
    for (i32 i = 0; i < 20; ++i)
    {
        mem_table.insert(i, std::to_string(i));
        std::cout << mem_table.at(i).second << '\n';
    }
    std::cout << mem_table.size() << ' '
              << mem_table.byteSize() << '\n';
    SSTable sstable(mem_table, 0);
    mem_table.clear();
    std::cout << mem_table.size() << ' '
              << mem_table.byteSize() << '\n';
    for (i32 i = 0; i < 25; ++i)
        std::cout << i << ':'
                  << sstable.find(i).second << '\n';

    std::cout << std::endl;

    printInfo("Test2");
    SSTable new_table("./data/level-0/0_0.sst");
    for (i32 i = 0; i < 25; ++i)
        std::cout << i << ':'
                  << new_table.find(i).second << '\n';

    std::cout << std::endl;

    printInfo("Test3");
    auto arr = new_table.toVector();
    for (const auto &entry : arr)
        std::cout << entry.first << ':'
                  << entry.second << '\n';
    new_table.removeTable();

    std::cout << std::endl;

    return 0;
}