#ifndef FDUPL_H
#define FDUPL_H

#include <stdint.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <queue>

#include "hasher.h"

namespace lib {

struct Dupl
{
    size_t file_size;
    std::vector<std::string> hashes;
    std::vector<std::string> equal_files;  // for reading bytes can be used first file from vector
};

using Dupls = std::vector<Dupl>;

using DuplMap = std::unordered_map<size_t, Dupls>;

using EqualFiles = std::vector<std::vector<std::string>>;

class Fdupl
{
public:
    Fdupl(
        std::vector<std::string> dirs,
        uint16_t block_size = 256,
        bool recursive = true,
        bool mt = true,
        std::string fileMask = "",
        Hasher::HashType hashAlgo = lib::Hasher::HashType::MD5);

    Fdupl& Start();
    EqualFiles GetResult();

private:
    void ProcessDir(boost::filesystem::path path);
    void ProcessFilesInDir(boost::filesystem::path path);
    void ProcessFile(boost::filesystem::path path);
    void CompareSameSizeFiles(boost::filesystem::path path, size_t fileSize);

    void StartPool();
    void StopPool();
    void PushDir(boost::filesystem::path path);
    void Worker();

private:
    std::vector<std::string> m_dirs{};
    size_t m_blockSize{};    // bytes to read for one time
    bool m_recursive{true};  // recursive processing dirs or not
    bool m_multithreading{true}; // multithread processing
    std::string m_fileMask{};
    Hasher m_hasher;

    mutable std::shared_mutex m_mutex_duplMap;
    DuplMap m_duplMap{};

    mutable std::mutex m_mutex_cv;
    std::condition_variable m_cv;
    std::queue<boost::filesystem::path> m_queue;
    bool m_stopThreads{false};

    unsigned int m_threadsNumber{1};
    std::vector<std::thread> m_threads;
};

}  // namespace lib

#endif  // FDBL_H
