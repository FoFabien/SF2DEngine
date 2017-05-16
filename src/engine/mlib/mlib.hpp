#ifndef MLIB_HPP
#define MLIB_HPP

// macro
#define cdelete(x) {delete x; x = NULL;}
#define adelete(x) {delete [] x; x = NULL;}

// declaration
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdint.h>

#if (!defined(PACK_BUILDER_BUILD) && !defined(EDITOR_BUILD))
#include <SFML/System/String.hpp>
#endif

namespace mlib
{
    /// bit manipulation
    bool getBit(const uint8_t *i, const uint8_t b);
    void toggleBit(uint8_t *i, const uint8_t b);
    void setBit(uint8_t *i, const uint8_t b);
    void clearBit(uint8_t *i, const uint8_t b);

    /// string convertion
    std::string int2str(const int32_t i);
    std::string int2hex(const int32_t i);
    std::string uint2str(const uint32_t i);
    std::string uint2hex(const uint32_t i);
    std::string long2str(const int64_t i);
    std::string long2hex(const int64_t i);
    std::string ulong2str(const uint64_t i);
    std::string ulong2hex(const uint64_t i);
    std::string float2str(const float i);
    std::string float2hex(const float i);

    std::string wstr2str(const std::wstring &i); // the wstring must contains an ansi string
    std::wstring str2wstr(const std::string &i);

    /// number convertion
    bool str2bool(const std::string &i);
    int8_t str2char(const std::string &i);
    int16_t str2short(const std::string &i);
    int32_t str2int(const std::string &i);
    int64_t str2long(const std::string &i);
    float str2float(const std::string &i);
    double str2double(const std::string &i);

    bool wstr2bool(const std::wstring &i);
    int8_t wstr2char(const std::wstring &i);
    int16_t wstr2short(const std::wstring &i);
    int32_t wstr2int(const std::wstring &i);
    int64_t wstr2long(const std::wstring &i);
    float wstr2float(const std::wstring &i);
    double wstr2double(const std::wstring &i);

    /// string manipulation
    std::vector<std::string> &split(const std::string &s, std::vector<char> delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, std::vector<char> delim);
    bool isNumber(std::string s);
    bool isFloat(std::string s);

    std::vector<std::wstring> &wsplit(const std::wstring &s, std::vector<wchar_t> delim, std::vector<std::wstring> &elems);
    std::vector<std::wstring> wsplit(const std::wstring &s, std::vector<wchar_t> delim);
    bool isNumber(std::wstring s);
    bool isFloat(std::wstring s);

    #if (!defined(PACK_BUILDER_BUILD) && !defined(EDITOR_BUILD))
    std::vector<sf::String> &sfsplit(const sf::String &s, std::vector<uint32_t> delim, std::vector<sf::String> &elems);
    std::vector<sf::String> sfsplit(const sf::String &s, std::vector<uint32_t> delim);
    bool isNumber(sf::String s);
    bool isFloat(sf::String s);
    sf::String sfsubstr(const sf::String &s, int32_t start, int32_t lenght);
    #endif

    /// stream & file manipulation
    std::istream& getline(std::istream& stream, std::string& str);
    std::wistream& wgetline(std::wistream& stream, std::wstring& str);
    bool copyFile(std::string i, std::string o);
    bool delFile(std::string f);
    uint32_t readUTF8char(std::istream& i);
    void writeUTF8char(std::ostream& o, uint32_t c);
    uint32_t fromUTF8(uint32_t i);
    uint32_t toUTF8(uint32_t i);

    /// others
    #ifndef EDITOR_BUILD
    std::vector<std::wstring> getFileList(const std::string &directory); // directory must be a relative path
    bool dirExist(const std::string &directory);
    bool makeDir(const std::string &directory);
    std::string getSavePath();
    std::wstring getWorkingDirectory();
    void setWorkingDirectory(const std::wstring &directory);
    #endif

    /// Log file class
    class MLog
    {
        public:
            MLog();
            ~MLog();
            void resetTimestamp(); // set timestamp to 0
            void setFile(std::string name, bool clr = false); // set the output file
            void setOutput(bool toCout, bool toFile); // set the targeted output
            void useTimestamp(bool b); // add automatically the timestamp at the output
            uint32_t t(); // give the timestamp (dec format)
            std::string ts(); // give the timestamp (string format)
            std::string tsf(); // give the timestamp (string format + ": ")
            MLog& operator=(std::string arg); // output
        protected:
            bool oToCout;
            bool oToFile;
            bool addT;
            std::string file;
            uint32_t timestamp;
    };
}

extern mlib::MLog Out;

#endif // MLIB_HPP
