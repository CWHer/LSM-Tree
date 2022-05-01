#include "memtable.hpp"

MemTable mem_table;
int main()
{
    printInfo("Test1");
    for (i32 i = 0; i < 20; ++i)
        mem_table.insert(i, std::to_string(i));
    std::cout << mem_table.size() << ' '
              << mem_table.byteSize() << '\n';

    printInfo("Test2");
    for (i32 i = 0; i < 20; ++i)
        mem_table.insert(i, std::to_string(20 - i));
    std::cout << mem_table.size() << ' '
              << mem_table.byteSize() << '\n';
    for (i32 i = 0; i < 20; ++i)
        std::cout << i << ':'
                  << mem_table.at(i).second << '\n';

    printInfo("Test3");
    for (i32 i = 0; i < 20; ++i)
        mem_table.insert(i, std::to_string(30 - i));
    std::cout << mem_table.size() << ' '
              << mem_table.byteSize() << '\n';
    for (i32 i = 0; i < 20; ++i)
        std::cout << i << ':'
                  << mem_table.at(i).second << '\n';

    std::cout << std::endl;
    return 0;
}