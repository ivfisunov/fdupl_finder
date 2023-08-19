#include "reader.h"

#include <iostream>

namespace lib {

Reader::Reader(boost::filesystem::path filePath) : m_filePath(filePath)
{
    m_ifs = std::ifstream{m_filePath, std::ios::binary};
}

Reader::~Reader()
{
    m_ifs.close();
}

std::string Reader::ReadBlock(size_t blockCount, size_t blockSize)
{
    std::string buffer(blockSize, '\0');
    m_ifs.seekg(blockCount * blockSize);
    m_ifs.read(&buffer[0], blockSize);
    return buffer;
}

}  // namespace lib