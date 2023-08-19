#ifndef READER_H
#define READER_H

#include "i_reader.h"

#include <boost/filesystem.hpp>

namespace lib {

class Reader : public lib::IReader
{
public:
    Reader(boost::filesystem::path filePath);
    ~Reader();

    std::string ReadBlock(size_t blocCount, size_t blockSize) override;

private:
    boost::filesystem::path m_filePath{};
    std::ifstream m_ifs{};
};

}  // namespace lib

#endif  // READER_H