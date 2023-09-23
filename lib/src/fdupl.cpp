#include <exception>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>

#include "fdupl.h"
#include "reader.h"
#include "hasher.h"

namespace lib {

Fdupl::Fdupl(
    std::vector<std::string> dirs,
    uint16_t blockSize,
    bool recursive,
    bool mt,
    std::string fileMask,
    Hasher::HashType hashAlgo)
    : m_dirs(dirs)
    , m_blockSize(blockSize)
    , m_recursive(recursive)
    , m_multithreading(mt)
    , m_fileMask(fileMask)
    , m_hasher(Hasher(hashAlgo))
{
    m_threadsNumber = std::thread::hardware_concurrency() < 2 ? 2 : std::thread::hardware_concurrency();
}

EqualFiles Fdupl::GetResult()
{
    EqualFiles ef{};
    for (const auto& entry : m_duplMap)
    {
        for (const auto& dupl : entry.second)
        {
            if (dupl.equal_files.size() >= 2)
                ef.push_back(dupl.equal_files);
        }
    }
    return ef;
}

Fdupl& Fdupl::Start()
{
    try
    {
        if (m_multithreading)
        {
            StartPool();
        }
        for (const auto& dir : m_dirs)
        {
            if (boost::filesystem::is_directory(dir))
            {
                m_multithreading ? PushDir(dir) : ProcessFilesInDir(dir);

                if (m_recursive)
                {
                    ProcessDir(dir);
                }
            }
        }
        if (m_multithreading)
        {
            StopPool();
        }
    }
    catch (const std::exception& e)
    {
        throw;
    }

    return *this;
}

void Fdupl::ProcessDir(boost::filesystem::path path)
{
    for (auto&& p : boost::filesystem::directory_iterator(path))
    {
        if (boost::filesystem::is_directory(p))
        {
            if (m_multithreading)
            {
                PushDir(p.path());
            }
            else
            {
                ProcessFilesInDir(p.path());
            }
            ProcessDir(p.path());
        }
    }
}

void Fdupl::ProcessFilesInDir(boost::filesystem::path path)
{
    for (auto&& p : boost::filesystem::directory_iterator(path))
    {
        if (boost::filesystem::is_regular_file(p))
        {
            ProcessFile(p.path());
        }
    }
}

void Fdupl::ProcessFile(boost::filesystem::path path)
{
    auto fileSize = static_cast<size_t>(boost::filesystem::file_size(path));

    {
        std::lock_guard l(m_mutex_duplMap);
        if (auto key = m_duplMap.find(fileSize); key == m_duplMap.end())
        {
            // key not found, create it
            Dupl dupl{fileSize, {}, {path.string()}};
            Dupls dupls{dupl};
            m_duplMap[fileSize] = dupls;
            return;
        }
    }
    // key found, compare
    CompareSameSizeFiles(path, fileSize);
}

void Fdupl::CompareSameSizeFiles(boost::filesystem::path path, size_t fileSize)
{
    std::vector<std::string> newFileHashes{};
    for (auto& dupl : m_duplMap[fileSize])
    {
        auto readBlockCount = fileSize < m_blockSize ? 1 :
            fileSize % m_blockSize == 0              ? (fileSize / m_blockSize) :
                                                       (fileSize / m_blockSize + 1);

        Reader prevFile{dupl.equal_files[0]};  // all files in vector are equal, so we can use first
        Reader newFile{path};
        bool filesEqual{true};
        for (size_t i = 0; i < readBlockCount; ++i)
        {
            std::string prevFileHash{};
            std::string newFileHash{};

            {
                std::lock_guard l(m_mutex_duplMap);
                if (i < dupl.hashes.size())
                {
                    // take hash from vector
                    prevFileHash = dupl.hashes[i];
                }
                else
                {
                    // calculate hash from file readed block
                    prevFileHash = m_hasher.GetHash(prevFile.ReadBlock(i, m_blockSize));
                    dupl.hashes.push_back(prevFileHash);
                }
            }

            // need to hash readed block
            if (i < newFileHashes.size())
            {
                newFileHash = newFileHashes[i];
            }
            else
            {
                newFileHash = m_hasher.GetHash(newFile.ReadBlock(i, m_blockSize));
                newFileHashes.push_back(newFileHash);
            }

            if (prevFileHash != newFileHash)
            {
                filesEqual = false;
                break;
            }
        }

        // if files are equal then push new file to vector and return from the function
        if (filesEqual)
        {
            std::lock_guard l(m_mutex_duplMap);
            dupl.equal_files.push_back(path.string());
            return;
        }
    }

    // we have totaly new file of the same size, so add new dupl
    std::lock_guard l(m_mutex_duplMap);
    m_duplMap[fileSize].push_back({fileSize, newFileHashes, {path.string()}});
}

void Fdupl::StartPool()
{
    for (int i = 0; i < m_threadsNumber; ++i)
    {
        m_threads.push_back(std::thread(&Fdupl::Worker, this));
    }
}

void Fdupl::StopPool()
{
    {
        std::lock_guard lock(m_mutex_cv);
        m_stopThreads = true;
    }
    m_cv.notify_all();

    for (auto& thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}


void Fdupl::PushDir(boost::filesystem::path path)
{
    {
        std::lock_guard lock(m_mutex_cv);
        m_queue.push(path);
    }
    m_cv.notify_all();
}

void Fdupl::Worker()
{
    while (true)
    {
        boost::filesystem::path path;
        {
            std::unique_lock l(m_mutex_cv);
            m_cv.wait(l, [this]() { return !m_queue.empty() || m_stopThreads; });
            if (m_queue.empty() && m_stopThreads)
            {
                return;
            }
            path = m_queue.front();
            m_queue.pop();
        }
        ProcessFilesInDir(path);
    }
}

}  // namespace lib
