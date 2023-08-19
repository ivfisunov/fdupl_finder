#ifndef FDUPL_H
#define FDUPL_H

#include <stdint.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

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
        uint16_t block_size = 64,
        bool recursive = true,
        std::string fileMask = "",
        Hasher::HashType hashAlgo = lib::Hasher::HashType::MD5);

    Fdupl& Start();
    EqualFiles GetResult();

private:
    void ProcessDir(boost::filesystem::path path);
    void ProcessFile(boost::filesystem::path path);
    void CompareSameSizeFiles(boost::filesystem::path path, size_t fileSize);

private:
    std::vector<std::string> m_dirs{};
    size_t m_blockSize{};    // bytes
    bool m_recursive{true};  // recursive processing dirs or not
    std::string m_fileMask{};
    Hasher m_hasher;

    DuplMap m_duplMap{};
};

}  // namespace lib

#endif  // FDBL_H
