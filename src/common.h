#pragma once

#include <iostream>
#include <bitset>
#include <string>
#include <exception>
#include <memory>
#include <utility>
#include <tuple>
#include <random>
#include <algorithm>
#include <vector>
#include <limits>
#include <list>
#include <map>

typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;

using std::bitset;
using std::list;
using std::make_pair;
using std::make_shared;
using std::map;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

const string DELETED_TOKEN = "~DELETED~";
const string EMPTY_TOKEN = "";

// >>>>>> utility functions
void printInfo(string msg);

void printWarning(bool expr, string msg);

void printError(bool expr, string msg);

// x ~ [min, max]
i32 randomInt(i32 min, i32 max);

// <<<<< utility functions