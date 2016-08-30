#include "mlib.hpp"
#include <iomanip>
#include <time.h>
#include <cstdio>
#include <iostream>

#ifdef WINDOWS_VERSION
    #include <windows.h>
    #include <shlobj.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#define F_PRECISION 9

mlib::MLog Out;

bool mlib::getBit(const uint8_t *i, const uint8_t b)
{
    if(!i) return false;
    return ((*i) >> b) & 1;
}

void mlib::toggleBit(uint8_t *i, const uint8_t b)
{
    if(!i) return;
    (*i) ^= 1 << b;
}

void mlib::setBit(uint8_t *i, const uint8_t b)
{
    if(!i) return;
    (*i) |= 1 << b;
}

void mlib::clearBit(uint8_t *i, const uint8_t b)
{
    if(!i) return;
    (*i) &= ~(1 << b);
}

std::string mlib::int2str(const int32_t i)
{
    std::stringstream o;
    o << i;
    return o.str();
}

std::string mlib::int2hex(const int32_t i)
{
    std::stringstream o;
    o << std::hex << i;
    return o.str();
}

std::string mlib::uint2str(const uint32_t i)
{
    std::stringstream o;
    o << i;
    return o.str();
}

std::string mlib::uint2hex(const uint32_t i)
{
    std::stringstream o;
    o << std::hex << i;
    return o.str();
}

std::string mlib::long2str(const int64_t i)
{
    std::stringstream o;
    o << i;
    return o.str();
}

std::string mlib::long2hex(const int64_t i)
{
    std::stringstream o;
    o << std::hex << i;
    return o.str();
}

std::string mlib::ulong2str(const uint64_t i)
{
    std::stringstream o;
    o << i;
    return o.str();
}

std::string mlib::ulong2hex(const uint64_t i)
{
    std::stringstream o;
    o << std::hex << i;
    return o.str();
}

std::string mlib::float2str(const float i)
{
    std::stringstream o;
    o << std::setprecision(F_PRECISION) << i;
    return o.str();
}

std::string mlib::float2hex(const float i)
{
    std::stringstream o;
    int32_t* b = ((int32_t*)&i);
    o << std::hex << *b;
    return o.str();
}

std::string mlib::wstr2str(const std::wstring &i)
{
    std::string o;
    for(size_t k = 0; k < i.size(); ++k) o += i[k] % 0x100;
    return o;
}

std::wstring mlib::str2wstr(const std::string &i)
{
    std::wstring o;
    for(size_t k = 0; k < i.size(); ++k) o += i[k];
    return o;
}

bool mlib::str2bool(const std::string &i)
{
    std::stringstream c(i);
    bool o;
    c >> o;
    return o;
}

int8_t mlib::str2char(const std::string &i)
{
    std::stringstream c(i);
    int8_t o;
    c >> o;
    return o;
}

int16_t mlib::str2short(const std::string &i)
{
    std::stringstream c(i);
    int16_t o;
    c >> o;
    return o;
}

int32_t mlib::str2int(const std::string &i)
{
    std::stringstream c(i);
    int32_t o;
    c >> o;
    return o;
}

int64_t mlib::str2long(const std::string &i)
{
    std::stringstream c(i);
    int64_t o;
    c >> o;
    return o;
}

float mlib::str2float(const std::string &i)
{
    std::stringstream c(i);
    float o;
    c >> o;
    return o;
}

double mlib::str2double(const std::string &i)
{
    std::stringstream c(i);
    double o;
    c >> o;
    return o;
}

bool mlib::wstr2bool(const std::wstring &i)
{
    std::wstringstream c(i);
    bool o;
    c >> o;
    return o;
}

int8_t mlib::wstr2char(const std::wstring &i)
{
    std::wstringstream c(i);
    int16_t o;
    c >> o;
    return (o % 0x100);
}

int16_t mlib::wstr2short(const std::wstring &i)
{
    std::wstringstream c(i);
    int16_t o;
    c >> o;
    return o;
}

int32_t mlib::wstr2int(const std::wstring &i)
{
    std::wstringstream c(i);
    int32_t o;
    c >> o;
    return o;
}

int64_t mlib::wstr2long(const std::wstring &i)
{
    std::wstringstream c(i);
    int64_t o;
    c >> o;
    return o;
}

float mlib::wstr2float(const std::wstring &i)
{
    std::wstringstream c(i);
    float o;
    c >> o;
    return o;
}

double mlib::wstr2double(const std::wstring &i)
{
    std::wstringstream c(i);
    double o;
    c >> o;
    return o;
}

std::vector<std::string> &mlib::split(const std::string &s, std::vector<char> delim, std::vector<std::string> &elems) {
    std::string item;
    bool ok;

    for(size_t i = 0; i < s.size(); ++i)
    {
        ok = true;
        for(uint32_t j = 0; j < delim.size() && ok; ++j)
            if(s[i] == delim[j]) ok = false;
        if(ok) item += s[i];
        else
        {
            if(!item.empty()) elems.push_back(item);
            item.clear();
        }
    }
    if(!item.empty()) elems.push_back(item);
    return elems;
}

std::vector<std::string> mlib::split(const std::string &s, std::vector<char> delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool mlib::isNumber(std::string s)
{
    if(s.empty()) return false;
    if(s.size() > 1 && s[0] == '-') s.erase(0);
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return (!s.empty() && it == s.end());
}

bool mlib::isFloat(std::string s)
{
    if(s.empty()) return false;
    if(s.size() > 1 && s[0] == '-') s.erase(0);
    if(s[0] < '0' || s[0] > '9') return false;
    std::string::const_iterator it = s.begin();
    bool dot = false;
    while (it != s.end() && (std::isdigit(*it) || *it == '.' || *it == ','))
    {
        if(*it == '.')
        {
            if(dot) return false;
            dot = true;
        }
        ++it;
    }
    return (!s.empty() && it == s.end());
}

std::vector<std::wstring> &mlib::wsplit(const std::wstring &s, std::vector<wchar_t> delim, std::vector<std::wstring> &elems)
{
    std::wstring item;
    bool ok;

    for(size_t i = 0; i < s.size(); ++i)
    {
        ok = true;
        for(uint32_t j = 0; j < delim.size() && ok; ++j)
            if(s[i] == delim[j]) ok = false;
        if(ok) item += s[i];
        else
        {
            if(!item.empty()) elems.push_back(item);
            item.clear();
        }
    }
    if(!item.empty()) elems.push_back(item);
    return elems;
}

std::vector<std::wstring> mlib::wsplit(const std::wstring &s, std::vector<wchar_t> delim)
{
    std::vector<std::wstring> elems;
    wsplit(s, delim, elems);
    return elems;
}

bool mlib::isNumber(std::wstring s)
{
    if(s.empty()) return false;
    if(s.size() > 1 && s[0] == '-') s.erase(0);
    std::wstring::const_iterator it = s.begin();
    while (it != s.end() && std::iswdigit(*it)) ++it;
    return (!s.empty() && it == s.end());
}

bool mlib::isFloat(std::wstring s)
{
    if(s.empty()) return false;
    if(s.size() > 1 && s[0] == '-') s.erase(0);
    if(s[0] < '0' || s[0] > '9') return false;
    std::wstring::const_iterator it = s.begin();
    bool dot = false;
    while (it != s.end() && (std::iswdigit(*it) || *it == '.' || *it == ','))
    {
        if(*it == '.')
        {
            if(dot) return false;
            dot = true;
        }
        ++it;
    }
    return (!s.empty() && it == s.end());
}

#if (!defined(PACK_BUILDER_BUILD) && !defined(EDITOR_BUILD))
std::vector<sf::String> &mlib::sfsplit(const sf::String &s, std::vector<uint32_t> delim, std::vector<sf::String> &elems)
{
    std::wstring item;
    bool ok;

    for(size_t i = 0; i < s.getSize(); ++i)
    {
        ok = true;
        for(uint32_t j = 0; j < delim.size() && ok; ++j)
            if(s[i] == delim[j]) ok = false;
        if(ok) item += s[i];
        else
        {
            if(!item.empty()) elems.push_back(item);
            item.clear();
        }
    }
    if(!item.empty()) elems.push_back(item);
    return elems;
}

std::vector<sf::String> mlib::sfsplit(const sf::String &s, std::vector<uint32_t> delim)
{
    std::vector<sf::String> elems;
    sfsplit(s, delim, elems);
    return elems;
}

bool mlib::isNumber(sf::String s)
{
    if(s.isEmpty()) return false;
    if(s.getSize() > 1 && s[0] == '-') s.erase(0);

    for(size_t i = 0; i < s.getSize(); ++i)
        if(s[i] < '0' || s[i] > '9') return false;
    return true;
}

bool mlib::isFloat(sf::String s)
{
    if(s.isEmpty()) return false;
    if(s.getSize() > 1 && s[0] == '-') s.erase(0);
    if(s[0] < '0' || s[0] > '9') return false;
    bool dot = false;

    for(size_t i = 0; i < s.getSize(); ++i)
    {
        if(s[i] == '.')
        {
            if(dot) return false;
            else dot = true;
        }
        else if(s[i] < '0' || s[i] > '9') return false;
    }
    return true;
}

sf::String mlib::sfsubstr(const sf::String &s, int32_t start, int32_t lenght)
{
    sf::String r;
    for(int32_t i = start; i < (int32_t)s.getSize() && i < start + lenght; ++i)
    {
        if(i >= 0) r += s[i];
    }
    return r;
}
#endif

std::istream& mlib::getline(std::istream& stream, std::string& str)
{
    std::getline(stream, str);
    if(str.length() > 0 && str[str.length()-1] == '\r')
    {
        str.erase(str.length()-1);
    }
    return stream;
}

std::wistream& mlib::wgetline(std::wistream& stream, std::wstring& str)
{
    std::getline(stream, str);
    if(str.length() > 0 && str[str.length()-1] == '\r')
    {
        str.erase(str.length()-1);
    }
    return stream;
}

bool mlib::copyFile(std::string i, std::string o)
{
    std::ifstream fi(i.c_str(), std::ios::in | std::ios::binary);
    std::ofstream fo(o.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!fi || !fo) return false;

    std::streampos fsize = fi.tellg();
    fi.seekg(0, std::ios::end);
    fsize = fi.tellg() - fsize;
    fi.seekg(0, std::ios::beg);

    if(fsize <= 0) return false;

    unsigned char *array = new unsigned char[fsize];
    for(size_t i = 0; i < fsize; ++i) fi.read((char*)&array[i], 1);
    fi.close();
    fo.write((char*)array, fsize);
    fo.close();
    delete [] array;
    return true;
}

bool mlib::delFile(std::string f)
{
    std::remove(f.c_str());
    return (!std::ifstream(f.c_str()));
}

uint32_t mlib::readUTF8char(std::istream& i)
{
    uint32_t result = 0;
    unsigned char c = 0;
    if(i.good())
    {
        i.read((char*)&c, 1);
        result += c;
        if(c > (unsigned char)0x7f)
        {
            if(c >= 0xC2 && c <= 0xDF) // 2 bytes
            {
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                result = (result % 0b1000000) + (((result / 0x100) % 0b100000) << 6);
            }
            else if(c >= 0xE0 && c <= 0xEF) // 3 bytes
            {
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                result = (result % 0b1000000) + (((result / 0x100) % 0b1000000) << 6) + (((result / 0x10000) % 0b10000) << 12);
            }
            else if(c >= 0xF0 && c <= 0xF4) // 4 bytes
            {
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                i.read((char*)&c, 1);
                result = (result << 8) + c;
                result = (result % 0b1000000) + (((result / 0x100) % 0b1000000) << 6)  + (((result / 0x10000) % 0b1000000) << 12) + (((result / 0x1000000) % 0b1000) << 18);
            }
        }
    }
    return result;
}

void mlib::writeUTF8char(std::ostream& o, uint32_t c)
{
    uint32_t tmp;
    if(o.good())
    {
        if(c >= 0x10000 && c <= 0x10FFFF)
        {
            tmp = ((0b10000000 + (c % 0b1000000)) << 24) +
            ((0b10000000 + ((c / 0b1000000) % 0b1000000)) << 16) +
            ((0b10000000 + ((c / 0b100000000000000) % 0b1000000)) << 8) +
            (0b11110000 + ((c / 0b1000000000000000000) % 0b1000));
            o.write((char*)&tmp, 4);
        }
        else if(c >= 0x800 && c <= 0xFFFF)
        {
            tmp = ((0b10000000 + (c % 0b1000000)) << 16) +
            ((0b10000000 + ((c / 0b1000000) % 0b1000000)) << 8) +
            (0b11100000 + ((c / 0b1000000000000) % 0b10000));
            o.write((char*)&tmp, 3);
        }
        else if(c >= 0x80 && c <= 0x7FF)
        {
            tmp = ((0b10000000 + (c % 0b1000000)) << 8) +
            (0b11000000 + ((c / 0b1000000) % 0b100000));
            o.write((char*)&tmp, 2);
        }
        else if(c <= 0x7F)
        {
            tmp = c % 0b10000000;
            o.write((char*)&tmp, 1);
        }
    }
}

uint32_t mlib::fromUTF8(uint32_t i)
{
    uint32_t result;
    char* c = (char*)&i;
    result = c[0];
    if(c[0] > 0x7f)
    {
        if(c[0] >= 0xC2 && c[0] <= 0xDF) // 2 bytes
        {
            result = (result << 8) + c[1];
            result = (result % 0b1000000) + (((result / 0x100) % 0b100000) << 6);
        }
        else if(c[0] >= 0xE0 && c[0] <= 0xEF) // 3 bytes
        {
            result = (result << 8) + c[1];
            result = (result << 8) + c[2];
            result = (result % 0b1000000) + (((result / 0x100) % 0b1000000) << 6) + (((result / 0x10000) % 0b10000) << 12);
        }
        else if(c[0] >= 0xF0 && c[0] <= 0xF4) // 4 bytes
        {
            result = (result << 8) + c[1];
            result = (result << 8) + c[2];
            result = (result << 8) + c[3];
            result = (result % 0b1000000) + (((result / 0x100) % 0b1000000) << 6)  + (((result / 0x10000) % 0b1000000) << 12) + (((result / 0x1000000) % 0b1000) << 18);
        }
    }
    return result;
}

uint32_t mlib::toUTF8(uint32_t i)
{
    uint32_t tmp = 0;
    if(i >= 0x10000 && i <= 0x10FFFF)
    {
        tmp = ((0b10000000 + (i % 0b1000000)) << 24) +
        ((0b10000000 + ((i / 0b1000000) % 0b1000000)) << 16) +
        ((0b10000000 + ((i / 0b100000000000000) % 0b1000000)) << 8) +
        (0b11110000 + ((i / 0b1000000000000000000) % 0b1000));
    }
    else if(i >= 0x800 && i <= 0xFFFF)
    {
        tmp = ((0b10000000 + (i % 0b1000000)) << 16) +
        ((0b10000000 + ((i / 0b1000000) % 0b1000000)) << 8) +
        (0b11100000 + ((i / 0b1000000000000) % 0b10000));
    }
    else if(i >= 0x80 && i <= 0x7FF)
    {
        tmp = ((0b10000000 + (i % 0b1000000)) << 8) +
        (0b11000000 + ((i / 0b1000000) % 0b100000));
    }
    else if(i <= 0x7F)
    {
        tmp = i % 0b10000000;
    }
    return tmp;
}

#ifndef EDITOR_BUILD
std::vector<std::wstring> mlib::getFileList(const std::string &directory)
{
    std::vector<std::wstring> out;

    std::string fulldir = directory;
    if(fulldir[fulldir.size()-1] == '/') fulldir[fulldir.size()-1] = '\\';
    else if(fulldir[fulldir.size()-1] != '\\') fulldir += '\\';

    #ifdef WINDOWS_VERSION
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    #ifdef NOT_WSTRING
    if ((dir = FindFirstFile(std::string(".\\" + fulldir + "*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
    #else
    if ((dir = FindFirstFile(std::wstring(L".\\" + str2wstr(fulldir) + L"*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
    #endif
        return out; /* No files found */

    do {
        #ifdef NOT_WSTRING
        const std::string file_name = file_data.cFileName;
        #else
        const std::wstring file_name = file_data.cFileName;
        #endif
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (file_name[0] == '.')
            continue;

        if (is_directory)
            continue;

        #ifdef NOT_WSTRING
        out.push_back(str2wstr(file_name));
        #else
        out.push_back(file_name);
        #endif
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
    #else // LINUX, NOT TESTED
    /*DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(fulldir);
    while ((ent = readdir(dir)) != NULL) {
        const string file_name = ent->d_name;
        const string full_file_name = fulldir + file_name;

        if (file_name[0] == '.')
            continue;

        if (stat(full_file_name.c_str(), &st) == -1)
            continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;

        if (is_directory)
            continue;

        out.push_back(full_file_name);
    }
    closedir(dir);*/
    #endif
    return out;
}

bool mlib::dirExist(const std::string &directory)
{
    std::string fulldir = directory;
    if(fulldir[fulldir.size()-1] == '/') fulldir[fulldir.size()-1] = '\\';
    else if(fulldir[fulldir.size()-1] != '\\') fulldir += '\\';

    #ifdef WINDOWS_VERSION
    DWORD ftyp = GetFileAttributesA(fulldir.c_str());
    if(ftyp == INVALID_FILE_ATTRIBUTES) return false;
    if(ftyp & FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
    #else
    struct stat info;
    if(stat(fulldir.c_str(), &info) != 0) return false;
    else if(info.st_mode & S_IFDIR) return true;
    else return false;
    #endif
}

bool mlib::makeDir(const std::string &directory)
{
    std::string fulldir = directory;
    if(fulldir[fulldir.size()-1] == '/') fulldir[fulldir.size()-1] = '\\';
    else if(fulldir[fulldir.size()-1] != '\\') fulldir += '\\';

    #ifdef WINDOWS_VERSION
        #ifdef NOT_WSTRING
        if(CreateDirectory(fulldir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) return true;
        #else
        if(CreateDirectory(str2wstr(fulldir).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) return true;
        #endif
    return false;
    #else
    if(mkdir(fulldir.c_str(), 0777)) return true;
    return false;
    #endif // WINDOWS_VERSION
}

std::string mlib::getSavePath()
{
    #ifdef WINDOWS_VERSION
    std::string str(getenv("PROGRAMDATA"));
    str += '\\';
    return str;
    #else

    #endif
}

std::wstring mlib::getWorkingDirectory()
{
    std::wstring str;
    #ifdef WINDOWS_VERSION
    #ifdef NOT_WSTRING
    char dir[MAX_PATH];
    #else
    wchar_t dir[MAX_PATH];
    #endif
    DWORD dwRet;
    dwRet = GetCurrentDirectory(MAX_PATH, dir);
    if(dwRet == 0) Out = "GetCurrentDirectory failed: " + mlib::int2str(GetLastError()) + "\n";
    else if(dwRet > MAX_PATH) Out = "Buffer too small. (: " + mlib::int2str(dwRet) + ")\n";
    else
    {
        for(size_t i = 0; i < MAX_PATH && dir[i] != '\0'; ++i)
            str+= dir[i];
    }
    #else

    #endif
    return str;
}

void mlib::setWorkingDirectory(const std::wstring &directory)
{
    #ifdef WINDOWS_VERSION
    #ifdef NOT_WSTRING
    if(!SetCurrentDirectory(wstr2str(directory).c_str()))
    #else
    if(!SetCurrentDirectory(directory.c_str()))
    #endif
    {
        Out = "SetCurrentDirectory failed: " + mlib::int2str(GetLastError()) + "\n";
    }
    #else

    #endif
}
#endif

mlib::MLog::MLog()
{
    oToCout = false;
    oToFile = false;
    addT = false;
    file = "Out.txt";
    timestamp = time(NULL);
}

mlib::MLog::~MLog()
{

}

void mlib::MLog::resetTimestamp()
{
    timestamp = time(NULL);
}

void mlib::MLog::setFile(std::string name, bool clr)
{
    file = name;
    if(clr) // clear the file
    {
        std::ofstream f(file.c_str(), std::ios::out | std::ios::trunc);
        if(f) f.close();
    }
}

void mlib::MLog::setOutput(bool toCout, bool toFile)
{
    oToCout = toCout;
    oToFile = toFile;
}

void mlib::MLog::useTimestamp(bool b)
{
    addT = b;
}

uint32_t mlib::MLog::t()
{
    return (uint32_t)(time(NULL) - timestamp);
}

std::string mlib::MLog::ts()
{
    return uint2str((uint32_t)(time(NULL) - timestamp));
}

std::string mlib::MLog::tsf()
{
    return uint2str((uint32_t)(time(NULL) - timestamp)) + ": ";
}

mlib::MLog& mlib::MLog::operator=(std::string arg)
{
    if(oToFile)
    {
        std::ofstream f(file.c_str(), std::ios::out | std::ios::app);
        if(f)
        {
            if(addT) f << t() << ": ";
            f << arg;
        }
    }
    if(oToCout)
    {
        if(addT) std::cout << t() << ": ";
        std::cout << arg;
    }
    return *this;
}
