#include "filesystem.hpp"
#ifdef _USE_FILESYSTEM_
#include "../../mlib/mcrypto.hpp"
#include "../../mlib/mlib.hpp"

#define FS_VERSION 0x32303076

#ifndef PACK_BUILDER_BUILD
#include <SFML/System/Lock.hpp>
FileSystem::FileSystem()
{
    mod = false;
    mod_folder = "";
    crypto = false;
}

FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::create(const std::string& pack, bool modEnabled, const std::string& modFolder)
{
    mutex.lock();
    mod = modEnabled;
    mod_folder = modFolder;
    mutex.unlock();
    clear();
    sf::Lock lock(mutex);
    Out = "FileSystem: Creating filesystem...\n";
    Out = "FileSystem: Base pack: \"" + pack + "\"\n";
    Out = "FileSystem: Modding state: " + mlib::int2str(mod) + "\n";
    Out = "FileSystem: Mod folder: \"" + mod_folder + "\"\n";
    if(!readHeader(pack))
    {
        clear();
        Out = "FileSystem: Failed to read header\n";
        return false;
    }

    if(mod)
    {
        std::vector<std::wstring> mod_list = mlib::getFileList(mod_folder);
        size_t ms = mod_list.size();
        Out = "FileSystem: " + mlib::uint2str(ms) + " mod pack(s) found\n";
        if(ms > 0)
        {
            for(size_t i = 0; i < ms; ++i)
            {
                if(!readHeader(mod_folder+mlib::wstr2str(mod_list[i]))) Out = "FileSystem: Failed to load the mod pack: \"" + mod_folder + mlib::wstr2str(mod_list[i]) + "\"\n";
                else Out = "FileSystem: Mod pack \"" + mod_folder + mlib::wstr2str(mod_list[i]) + "\" loaded\n";
            }
        }
    }
    Out = "FileSystem: Creation success\n";
    Out = "Filesystem: " + mlib::uint2str(getFileCount()) + " file(s)\n";
    Out = "Filesystem: " + mlib::ulong2str(getByteSize()) + " byte(s)\n";
    return true;
}

bool FileSystem::readHeader(const std::string& pack)
{
    std::ifstream f(pack.c_str(), std::ios::in | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_READ);
    fc.enable(crypto);
    fc.readSeed(f);

    std::streampos fsize = f.tellg();
    fc.seekg(f, 0, f.end);
    fsize = f.tellg() - fsize;
    fc.seekg(f, 0, f.beg);

    if(fsize < 8) return false; // minimal size

    uint32_t tmp32, fcount;
    uint8_t tmp8;

    fc.read(f, (char*)&tmp32, 4);
    if(tmp32 != FS_VERSION) return false;
    fc.read(f, (char*)&fcount, 4);
    packs.push_back(pack);

    for(uint32_t i = 0; i < fcount; ++i)
    {
        DataContainer tmp;
        std::string name;

        if(!fc.good(f)) return false;
        name.clear();
        do
        {
            if(!fc.good(f)) return false;
            fc.read(f, (char*)&tmp8, 1);
            if(tmp8 != 0x00) name += tmp8;
        }while(tmp8 != 0x00);

        fc.read(f, (char*)&tmp.size, 4);
        fc.read(f, (char*)&tmp32, 4);
        tmp.offset = tmp32;
        tmp.pack = &(packs.back());
        files[name] = tmp;
    }

    if(crypto)
    {
        for(auto &i: files)
        {
            fc.seekg(f, i.second.offset);
            i.second.seed = fc.getCurrentSeed();
        }
    }
    return true;
}

void FileSystem::clear()
{
    mutex.lock();
    for(auto &i : files)
    {
        delete [] (char*)(i.second.data);
        delete i.second.pack;
    }
    files.clear();
    packs.clear();
    mutex.unlock();
}

bool FileSystem::loadFile(const std::string &filename)
{
    sf::Lock lock(mutex);
    if(files.find(filename) == files.end()) return false;
    if(files[filename].data != nullptr) return true;
    std::ifstream f(files[filename].pack->c_str(), std::ios::in | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_READ);
    fc.enable(crypto);
    fc.readSeed(f);

    f.seekg(files[filename].offset);
    fc.setCurrentSeed(files[filename].seed);

    for(uint32_t i = 0; i < files[filename].size; ++i)
    {
        if(files[filename].size == 0) return true;
        files[filename].data = (void*)new char[files[filename].size];
        fc.read(f, (char*)files[filename].data, files[filename].size);
        return true;
    }
    return false;
}

bool FileSystem::exist(const std::string &filename)
{
    if(files.find(filename) == files.end()) return false;
    return true;
}

void FileSystem::unloadFile(const std::string &filename)
{
    mutex.lock();
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end() && it->second.nUse <= 0)
    {
        delete [] (char*)(it->second.data);
        files.erase(it);
    }
    mutex.unlock();
}

size_t FileSystem::getFileCount()
{
    return files.size();
}

uint64_t FileSystem::getByteSize()
{
    uint64_t size = 0;
    for(auto &i : files)
    {
        size += i.second.size;
    }
    return size;
}

void FileSystem::enableMod(bool enable)
{
    mod = enable;
}

bool FileSystem::isModEnabled()
{
    return mod;
}

void FileSystem::setModFolder(const std::string &folder)
{
    mod_folder = folder;
}

std::string FileSystem::getModFolder() const
{
    return mod_folder;
}

sf::MemoryInputStream FileSystem::getStream(const std::string &filename)
{
    mutex.lock();
    sf::MemoryInputStream stream;
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end() && it->second.data)
    {
        stream.open(it->second.data, it->second.size);
    }
    mutex.unlock();
    return stream;
}

DataContainer FileSystem::getData(const std::string &filename)
{
    mutex.lock();
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end() && it->second.data)
    {
        return it->second;
    }
    mutex.unlock();
    return DataContainer();
}

std::stringstream& FileSystem::getStringStream(const std::string &filename, std::stringstream &ss)
{
    mutex.lock();
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end() && it->second.data)
    {
        ss.write((char*)(it->second.data), it->second.size);
        ss.seekp(ss.beg);
    }
    mutex.unlock();
    return ss;
}

void FileSystem::lockFile(const std::string &filename)
{
    mutex.lock();
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end())
    {
        ++it->second.nUse;
    }
    mutex.unlock();
}

void FileSystem::unlockFile(const std::string &filename)
{
    mutex.lock();
    std::map<std::string, DataContainer>::iterator it = files.find(filename);
    if(it != files.end())
    {
        --it->second.nUse;
    }
    mutex.unlock();
}

void FileSystem::useCryto(bool enable)
{
    crypto = enable;
}
#else
#include <cstdlib>
#include <ctime>
#include "../../mlib/mrand.hpp"
int32_t FileSystem::buildPack(std::string pack, std::vector<std::string> filelist, bool crypto)
{
    uint32_t errcount = 0;
    uint8_t progress = 1;

    std::ofstream f(pack.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return -1;
    MCrypto fc(C_WRITE);
    fc.enable(crypto);
    mrand::setSeed(time(NULL));
    int32_t seed = mrand::rand();
    fc.setSeed(seed);

    uint32_t tmp32;

    tmp32 = FS_VERSION;
    fc.write(f, (char*)&tmp32, 4);
    tmp32 = filelist.size();
    fc.write(f, (char*)&tmp32, 4);

    std::vector<uint32_t> positions;
    std::vector<uint32_t> sizes;

    for(size_t i = 0; i < filelist.size(); ++i)
    {
        std::ifstream g(filelist[i].c_str(), std::ios::in | std::ios::binary);
        if(g)
        {
            std::streampos fsize = g.tellg();
            g.seekg(0, f.end);
            fsize = g.tellg() - fsize;
            g.seekg(0, f.beg);

            if(fsize < 0x100000000)
            {
                fc.write(f, filelist[i].c_str(), filelist[i].size()); // write the name
                tmp32 = 0;
                fc.write(f, (char*)&tmp32, 1); // write a 0
                sizes.push_back(fsize);
                fc.write(f, (char*)&fsize, 4); // write the size
                positions.push_back(f.tellp());
                fc.write(f, (char*)&tmp32, 4); // write a 0 (reserved for offset)
            }
            else
            {
                ++errcount;
                Out = "Error: Can't open " + filelist[i] + ", size not supported\n";
                filelist.erase(filelist.begin()+i);
                --i;
            }
            g.close();
        }
        else
        {
            ++errcount;
            g.close();
            Out = "Error: " + filelist[i] + " not found\n";
            filelist.erase(filelist.begin()+i);
            --i;
        }
    }

    for(size_t i = 0; i < filelist.size(); ++i)
    {
        Out = "Writing file: \"" + filelist[i] + "\"...\n";
        while(((i+1) * 10) / filelist.size() >= progress)
        {
            Out = "Progress " + mlib::int2str((progress-1)*10) + "%\n";
            ++progress;
        }
        tmp32 = f.tellp();
        fc.seekp(f, std::streampos(positions[i])); // go back and write file offset
        fc.write(f, (char*)&tmp32, 4);
        fc.seekp(f, std::streampos(tmp32));
        std::ifstream g(filelist[i].c_str(), std::ios::in | std::ios::binary);
        char c;
        if(g)
        {
            for(size_t j = 0; j < sizes[i]; ++j)
            {
                g.read(&c, 1);
                fc.write(f, &c, 1);
            }
        }
        g.close();
    }
    fc.writeSeed(f);
    f.close();
    return errcount;
}
#endif
#endif
