#ifndef MCRYPTO_HPP
#define MCRYPTO_HPP

#include <fstream>
#include <stdint.h>

#define C_WRITE true
#define C_READ false

class MCrypto
{
    public:
        MCrypto(bool type);
        ~MCrypto();
        void enable(bool bo);
        /// write
        void setSeed(int32_t s);
        void write(std::ostream &f, const char* ptr, size_t n);
        void writeSeed(std::ostream &f);
        void seekp(std::ostream &f, std::streampos pos);
        void seekp(std::ostream &f, std::streamoff off, std::ios_base::seekdir way);
        /// read
        void readSeed(std::istream &f);
        void read(std::istream &f, char* ptr, size_t n);
        void seekg(std::istream &f, std::streampos pos);
        void seekg(std::istream &f, std::streamoff off, std::ios_base::seekdir way);
        /// check
        bool good(std::istream &f);
        bool good(std::ostream &f);
        /// advanced use
        void setCurrentSeed(int32_t k);
        int32_t getCurrentSeed();
    protected:
        bool mode;
        bool enabled;
        int32_t seed;
        int32_t key;
        std::streampos endRead;

        inline char wXorChar(const char &c);
        inline void rXorChar(char* c, size_t n);
        inline void nXor(size_t n);
};

#endif // MCRYPTO_HPP
