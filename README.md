# LSM-Tree

:hugs: Implementation of basic LSM Tree (Log-Structured Merge Tree)

> Project of SE-2322 Advanced Data Structure @SJTU-SE



## Getting Started

- `correctness`&`persistence`

  ```bash
  make all
  ./correctness
  ./persistence
  ```

- Unit Test

  ```bash
  make test_filter
  make test_skiplist
  make test_memtable
  make test_sstable
  # then run the corresponding executables
  ```

  

## Features

Basic operations

```c++
KVStore(const std::string &dir);
~KVStore();
void put(uint64_t key, const std::string &s) override;
std::string get(uint64_t key) override;
bool del(uint64_t key) override;
void reset() override;
```



## TODO

- [ ] `SCAN(K1,K2)`

- [ ] `write-ahead-log`