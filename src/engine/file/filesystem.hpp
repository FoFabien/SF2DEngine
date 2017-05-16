#define _USE_FILESYSTEM_
#ifdef _USE_FILESYSTEM_
#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdint.h>

#ifndef PACK_BUILDER_BUILD
#include <SFML/System/Mutex.hpp>
#include <SFML/System/MemoryInputStream.hpp>

struct DataContainer
{
    int32_t nUse = 0;
    size_t size = 0;
    std::streampos offset = 0;
    void *data = nullptr;
    std::string *pack = nullptr;
    int32_t seed = 0;
};

class FileSystem
{
    public:
        FileSystem();
        ~FileSystem();
        bool create(const std::string& pack, bool modEnabled = false, const std::string& modFolder = "");
        void clear();
        bool loadFile(const std::string &filename);
        bool exist(const std::string &filename);
        void unloadFile(const std::string &filename);
        size_t getFileCount();
        uint64_t getByteSize();
        void enableMod(bool enable); /// authorize the modding
        bool isModEnabled();
        void setModFolder(const std::string &folder);
        std::string getModFolder() const;
        void useCryto(bool enable);

        sf::MemoryInputStream getStream(const std::string &filename);
        DataContainer getData(const std::string &filename);
        std::stringstream& getStringStream(const std::string &filename, std::stringstream &ss);

        void lockFile(const std::string &filename);
        void unlockFile(const std::string &filename);
    protected:
        bool readHeader(const std::string& pack);

        std::map<std::string, DataContainer> files;
        std::vector<std::string> packs;
        bool mod;
        bool crypto;
        std::string mod_folder;

        sf::Mutex mutex;
};
#else // used for the standalone "data pack builder"
namespace FileSystem
{
    int32_t buildPack(std::string pack, std::vector<std::string> filelist, bool crypto);
};
#endif

#endif // FILESYSTEM_HPP
#endif
