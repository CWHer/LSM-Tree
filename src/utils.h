#pragma once

#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#include <stdio.h>
#include <io.h>
#endif

namespace utils
{
    /**
     * Check whether directory exists
     * @param path directory to be checked.
     * @return ture if directory exists, false otherwise.
     */
    bool dirExists(std::string path);

    /**
     * list all filename in a directory
     * @param path directory path.
     * @return name of all files.
     */
    std::vector<std::string> scanDir(std::string path);

    /**
     * Create directory
     * @param path directory to be created.
     * @return 0 if directory is created successfully, -1 otherwise.
     */
    int _mkdir(const char *path);

    /**
     * Create directory recursively
     * @param path directory to be created.
     * @return 0 if directory is created successfully, -1 otherwise.
     */
    int mkdir(const char *path);

    /**
     * Delete a empty directory
     * @param path directory to be deleted.
     * @return 0 if delete successfully, -1 otherwise.
     */
    int rmdir(const char *path);

    /**
     * Delete a file
     * @param path file to be deleted.
     * @return 0 if delete successfully, -1 otherwise.
     */
    int rmfile(const char *path);
}