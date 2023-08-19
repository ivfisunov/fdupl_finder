#include "hasher.h"

#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace lib {

Hasher::Hasher(HashType hashType) : m_hashType(hashType = HashType::MD5) {}

Hasher::~Hasher() {}

std::string Hasher::GetHash(std::string buffer)
{
    switch (m_hashType)
    {
        case HashType::MD5:
            return GetMD5Hash(buffer);
            break;

        default:
            return {};
            break;
    }
}

std::string Hasher::GetMD5Hash(std::string buffer)
{
    MD5_CTX ctx;
    unsigned char md5digest[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(buffer.data()), buffer.size(), md5digest);

    std::stringstream ss{};
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) << md5digest[i];
    }
    return ss.str();
}

}  // namespace lib