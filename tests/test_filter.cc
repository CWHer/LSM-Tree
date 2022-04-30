#include "bloomfilter.hpp"

BloomFilter filter;

int main()
{
    for (i32 i = 0; i < 10; ++i)
        filter.insert(i);

    for (i32 i = -5; i < 15; ++i)
        std::cout << i << ':'
                  << filter.query(i) << '\n';
    std::cout << std::endl;
    return 0;
}