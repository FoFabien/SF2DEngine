#ifndef MLOAD_HPP
#define MLOAD_HPP

#include <string>
#include <sstream>
#include <fstream>

class MLoad // standard loading class. Must be derivated.
{
    public:
        MLoad();
        virtual ~MLoad();

        bool loadFromMemory(std::string file, bool tryFromFile = true); // load a file from the memory. And from the drive if not found in memory.
        bool loadFromFile(std::string file); // load a file from the hard drive
        std::string getName() const {return filename;} // return the file name
    protected:
        virtual bool load(std::istream &f) =0;
        std::string filename;
};

#endif // MLOAD_HPP
