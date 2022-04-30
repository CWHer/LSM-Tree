#include "skiplist.hpp"

SkipList<i32, i32> skip_list;
int main()
{
    printInfo("Test1");
    for (i32 i = 0; i < 20; ++i)
        skip_list.insert(i, i);
    skip_list.display();

    printInfo("Test2");
    for (i32 i = 0; i < 20; i += 2)
    {
        std::cout << i << ':'
                  << skip_list.at(i).second << '\n';
        skip_list.remove(i);
        // skip_list.display();
    }
    for (i32 i = 0; i < 20; i += 3)
    {
        std::cout << i << ':'
                  << skip_list.at(i).first << '\n';
    }
    std::cout << skip_list.size() << '\n';
    skip_list.clear();

    printInfo("Test3");
    for (i32 i = 0; i < 20; ++i)
        skip_list.insert(i, 20 - i);
    std::cout << skip_list.size() << '\n';
    for (i32 i = 0; i < 20; ++i)
        std::cout << i << ':'
                  << skip_list.at(i).second << '\n';

    printInfo("Test4");
    for (i32 i = 0; i < 20; ++i)
        skip_list.insert(i, 30 - i);
    std::cout << skip_list.size() << '\n';
    for (i32 i = 0; i < 20; ++i)
        std::cout << i << ':'
                  << skip_list.at(i).second << '\n';

    std::cout << std::endl;
    return 0;
}