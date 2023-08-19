#ifndef I_READER_H
#define I_READER_H

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace lib {

class IReader
{
public:
    virtual ~IReader(){};
    virtual std::string ReadBlock(size_t blocCount, size_t blockSize) = 0;
};

}  // namespace lib

#endif  // I_READER_H
