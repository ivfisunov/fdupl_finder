#ifndef I_HASHER_H
#define I_HASHER_H

#include <string>

namespace lib {

class IHasher
{
public:
    virtual ~IHasher() {};

    virtual std::string GetHash(std::string buffer) = 0;
};

}  // namespace lib

#endif  // I_HASHER_H