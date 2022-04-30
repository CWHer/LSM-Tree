#ifndef __COMMON_H__
#define __COMMON_H__

#include <iostream>
#include <bitset>
#include <string>
#include <exception>

typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;

using std::bitset;
using std::string;

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

#endif