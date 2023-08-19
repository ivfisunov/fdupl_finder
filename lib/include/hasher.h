#ifndef HASHER_H
#define HASHER_H

#include "i_hasher.h"

namespace lib {

class Hasher : public IHasher
{
public:
    enum class HashType
    {
        MD5,
        SHA256,
        SHA512
    };

    Hasher(HashType hashType);
    ~Hasher();

    std::string GetHash(std::string buffer) override;

private:
    std::string GetMD5Hash(std::string buffer);

private:
    HashType m_hashType{};
};

}  // namespace lib

#endif  // HASHER_H