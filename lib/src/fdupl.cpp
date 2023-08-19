#include <exception>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>

#include "fdupl.h"
#include "reader.h"
#include "hasher.h"

namespace lib {

Fdupl::Fdupl(
    std::vector<std::string> dirs, uint16_t blockSize, bool recursive, std::string fileMask, Hasher::HashType hashAlgo)
    : m_dirs(dirs), m_blockSize(blockSize), m_recursive(recursive), m_fileMask(fileMask), m_hasher(Hasher(hashAlgo))
{}

Fdupl& Fdupl::Start()
{
    try
    {
        for (const auto& dir : m_dirs)
        {
            if (boost::filesystem::is_directory(dir))
            {
                ProcessDir(dir);
            }
        }
    }
    catch (const std::exception& e)
    {
        throw;
    }

    return *this;
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

void Fdupl::ProcessDir(boost::filesystem::path path)
{
    for (auto&& p : boost::filesystem::directory_iterator(path))
    {
        if (boost::filesystem::is_directory(p))
        {
            if (m_recursive)
            {
                ProcessDir(p.path());
            }
        }
        else if (boost::filesystem::is_regular_file(p))
        {
            ProcessFile(p.path());
        }
    }
}

void Fdupl::ProcessFile(boost::filesystem::path path)
{
    auto fileSize = static_cast<size_t>(boost::filesystem::file_size(path));

    if (auto key = m_duplMap.find(fileSize); key != m_duplMap.end())
    {
        // key found, compare
        CompareSameSizeFiles(path, fileSize);
    }
    else
    {
        // key not found, create it
        Dupl dupl{fileSize, {}, {path.string()}};
        Dupls dupls{dupl};
        m_duplMap[fileSize] = dupls;
    }
}

void Fdupl::CompareSameSizeFiles(boost::filesystem::path path, size_t fileSize)
{
    auto dupls = m_duplMap[fileSize];
    std::vector<std::string> newFileHashes{};
    for (auto& dupl : dupls)
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
            if (i < dupl.hashes.size())
            {
                // take hash from vector
                prevFileHash = dupl.hashes[i];
            }
            else
            {
                // take hash from file readed block
                prevFileHash = m_hasher.GetHash(prevFile.ReadBlock(i, m_blockSize));
                dupl.hashes.push_back(prevFileHash);
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
            dupl.equal_files.push_back(path.string());
            m_duplMap[fileSize] = dupls;
            return;
        }
    }

    // we have totaly new file of the same size, so add new dupl
    dupls.push_back({fileSize, newFileHashes, {path.string()}});
    m_duplMap[fileSize] = dupls;
}

}  // namespace lib
