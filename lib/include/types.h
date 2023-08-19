#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <vector>
#include <string>
#include <unordered_map>

namespace lib {

struct Dupl
{
    size_t file_size;
    std::vector<std::string> hashes;
    std::vector<std::string> equal_files; // for reading bytes can be used first file from vector
};

using Dupls = std::vector<Dupl>;

using DuplMap = std::unordered_map<size_t, Dupls>;

}  // namespace lib

#endif
