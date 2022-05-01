#include "common.h"

void printInfo(string msg)
{
    std::cout << "[INFO]: " << msg << std::endl;
}

void printWarning(bool expr, string msg)
{
    if (expr)
        std::cout << "[WARN]: " << msg << std::endl;
}

void printError(bool expr, string msg)
{
    if (expr)
    {
        std::cout << "[ERROR]: " << msg << std::endl;
        throw std::runtime_error("error");
    }
}

// x ~ [min, max]
i32 randomInt(i32 min, i32 max)
{
    static std::mt19937 gen(131);
    std::uniform_int_distribution<i32> dist(min, max);
    return dist(gen);
}