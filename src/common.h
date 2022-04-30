#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <bitset>
#include <string>
#include <exception>
#include <memory>
#include <utility>
#include <tuple>
#include <random>

typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;

using std::bitset;
using std::make_pair;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::string;
using std::weak_ptr;

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

#endif