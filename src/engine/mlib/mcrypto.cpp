#include "mcrypto.hpp"

#define CRYPTO_VERSION 0x31304543
#define C_CALC 15897987

MCrypto::MCrypto(bool type)
{
    mode = type;
    seed = 0;
    key = 0;
    enabled = true;
    endRead = 0;
}

MCrypto::~MCrypto()
{

}

void MCrypto::enable(bool bo)
{
    enabled = bo;
}

void MCrypto::setSeed(int32_t s)
{
    if(!mode) return;
    key = s;
    seed = s;
}

void MCrypto::write(std::ostream &f, const char* ptr, size_t n)
{
    if(!mode) return;
    for(uint32_t i = 0; i < n; ++i)
    {
        char c;
        if(enabled) c = wXorChar(ptr[i]);
        else c = ptr[i];
        f.write(&c, 1);
    }
}

void MCrypto::writeSeed(std::ostream &f)
{
    if(!mode || !f || !enabled) return;
    int32_t tmp = seed ^ CRYPTO_VERSION;
    f.write((char*)&tmp, 4);
}

void MCrypto::seekp(std::ostream &f, std::streampos pos)
{
    if(!enabled) f.seekp(pos);
    else
    {
        std::streamoff tmp = f.tellp();
        f.seekp(pos);
        tmp = f.tellp() - tmp;
        if(tmp == std::streamoff(0)) return;
        else if(tmp > std::streamoff(0)) // forward
        {
            nXor(tmp);
        }
        else // backward
        {
            key = seed;
            nXor(f.tellp());
        }
    }
}

void MCrypto::seekp(std::ostream &f, std::streamoff off, std::ios_base::seekdir way)
{
    if(!enabled) f.seekp(off, way);
    else
    {
        std::streamoff tmp = f.tellp();
        f.seekp(off, way);
        tmp = f.tellp() - tmp;
        if(tmp == std::streamoff(0)) return;
        else if(tmp > std::streamoff(0)) // forward
        {
            nXor(tmp);
        }
        else // backward
        {
            key = seed;
            nXor(f.tellp());
        }
    }
}

void MCrypto::readSeed(std::istream &f)
{
    if(mode || !enabled) return;

    f.seekg(-4, f.end);
    endRead = f.tellg();
    int32_t tmp;
    f.read((char*)&tmp, 4);
    seed = tmp ^ CRYPTO_VERSION;
    key = seed;
    f.seekg(0, f.beg);
}

void MCrypto::read(std::istream &f, char* ptr, size_t n)
{
    if(mode) return;

    f.read(ptr, n);
    if(enabled) rXorChar(ptr, n);
}

void MCrypto::seekg(std::istream &f, std::streampos pos)
{
    if(!enabled) f.seekg(pos);
    else
    {
        std::streamoff tmp = f.tellg();
        f.seekg(pos);
        if(f.tellg() > endRead) f.seekg(endRead-std::streamoff(1));
        tmp = f.tellg() - tmp;
        if(tmp == std::streamoff(0)) return;
        else if(tmp > std::streamoff(0)) // forward
        {
            nXor(tmp);
        }
        else // backward
        {
            key = seed;
            nXor(f.tellg());
        }
    }
}

void MCrypto::seekg(std::istream &f, std::streamoff off, std::ios_base::seekdir way)
{
    if(!enabled) f.seekg(off, way);
    else
    {
        std::streamoff tmp = f.tellg();
        f.seekg(off, way);
        if(f.tellg() > endRead) f.seekg(endRead-std::streamoff(1));
        tmp = f.tellg() - tmp;
        if(tmp == std::streamoff(0)) return;
        else if(tmp > std::streamoff(0)) // forward
        {
            nXor(tmp);
        }
        else // backward
        {
            key = seed;
            nXor(f.tellg());
        }
    }
}

bool MCrypto::good(std::istream &f)
{
    if(f.tellg() == endRead) return false;
    return f.good();
}

bool MCrypto::good(std::ostream &f)
{
    return f.good();
}

void MCrypto::setCurrentSeed(int32_t k)
{
    key = k;
}

int32_t MCrypto::getCurrentSeed()
{
    return key;
}

char MCrypto::wXorChar(const char &c)
{
    key = key + C_CALC;
    return c ^ key;
}

void MCrypto::rXorChar(char* c, size_t n)
{
    for(size_t i = 0; i < n; ++i)
    {
        key = key + C_CALC;
        c[i] = c[i] ^ key;
    }
}

void MCrypto::nXor(size_t n)
{
    for(size_t i = 0; i < n; ++i)
    {
        key = key + C_CALC;
    }
}
