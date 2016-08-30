#include "inifile.hpp"

#ifdef _USE_INIFILE_
#include "../../mlib/mlib.hpp"

IniFile::IniFile()
{
    //ctor
}

IniFile::~IniFile()
{
    //dtor
}

void IniFile::editStrData(const sf::String &name, const sf::String &data, const sf::String &category)
{
    if(datas.find(category) == datas.end()) datas[category] = std::map<sf::String, sf::String>();
    datas[category][name] = data;
}

void IniFile::editBoolData(const sf::String &name, const bool &data, const sf::String &category)
{
    if(datas.find(category) == datas.end()) datas[category] = std::map<sf::String, sf::String>();
    datas[category][name] = mlib::int2str(data);
}

void IniFile::editIntData(const sf::String &name, const int32_t &data, const sf::String &category)
{
    if(datas.find(category) == datas.end()) datas[category] = std::map<sf::String, sf::String>();
    datas[category][name] = mlib::int2str(data);
}

void IniFile::editFloatData(const sf::String &name, const float &data, const sf::String &category)
{
    if(datas.find(category) == datas.end()) datas[category] = std::map<sf::String, sf::String>();
    datas[category][name] = mlib::float2str(data);
}

sf::String IniFile::getStrData(const sf::String &name, const sf::String &category)
{
    if(datas.find(category) == datas.end() || datas[category].find(name) == datas[category].end()) return sf::String("");
    return datas[category][name];
}

bool IniFile::getBoolData(const sf::String &name, const sf::String &category)
{
    if(datas.find(category) == datas.end() || datas[category].find(name) == datas[category].end()) return false;
    return mlib::str2bool(datas[category][name]);
}

int32_t IniFile::getIntData(const sf::String &name, const sf::String &category)
{
    if(datas.find(category) == datas.end() || datas[category].find(name) == datas[category].end()) return 0;
    return mlib::str2int(datas[category][name]);
}

float IniFile::getFloatData(const sf::String &name, const sf::String &category)
{
    if(datas.find(category) == datas.end() || datas[category].find(name) == datas[category].end()) return 0;
    return mlib::str2float(datas[category][name]);
}

bool IniFile::exist(const sf::String &name, const sf::String &category)
{
    if(datas.find(category) == datas.end() || datas[category].find(name) == datas[category].end()) return false;
    return true;
}

void IniFile::erase(const sf::String &name, const sf::String &category)
{
    std::map<sf::String, std::map<sf::String, sf::String> >::iterator it = datas.find(category);
    if(it != datas.end())
    {
        std::map<sf::String, sf::String>::iterator jt = it->second.find(name);
        if(jt != it->second.end()) it->second.erase(jt);
    }
}

bool IniFile::load(const std::string &file)
{
    std::ifstream f(file.c_str(), std::ios::in | std::ios::binary);
    if(!f) return false;
    uint32_t tmp_char = 0;
    char c;
    bool utf8 = false;
    f.read((char*)&tmp_char, 3);
    if(tmp_char == 0xbfbbef) utf8 = true;
    else f.seekg(f.beg);
    if(!f.good()) return false;

    datas.clear();
    sf::String tmp_str;
    sf::String current_category = "";
    if(datas.find("") == datas.end()) datas[""] = std::map<sf::String, sf::String>();


    while(f.good())
    {
        tmp_str.clear();
        if(utf8)
        {
            do
            {
                tmp_char = mlib::readUTF8char(f);
                if(tmp_char != 0x0000000a /* \n */&& tmp_char != 0x0000000d /* \d */&& f.good()) tmp_str += sf::String(tmp_char);
            }while(tmp_char != 0x0000000a /* \n */ && f.good());
        }
        else
        {
            do
            {
                f.read(&c, 1);
                if(c != '\n' && c != '\r' && f.good()) tmp_str += sf::String(c);
            }while(c != '\n' && f.good());
        }

        if(tmp_str.getSize() > 2 && tmp_str[0] == '[' && tmp_str[tmp_str.getSize()-1] == ']')
        {
            tmp_str = tmp_str.substring(1, tmp_str.getSize()-2);
            if(datas.find(tmp_str) == datas.end()) datas[tmp_str] = std::map<sf::String, sf::String>();
            current_category = tmp_str;
        }
        else if(tmp_str.getSize() > 1)
        {
            std::vector<sf::String> elems;
            mlib::sfsplit(tmp_str, {'='}, elems);
            if(elems.size() == 1) datas[current_category][elems[0]] = sf::String("");
            else
            {
                for(size_t i = 2; i < elems.size(); ++i) elems[1] = elems[1] + "=" + elems[i];
                datas[current_category][elems[0]] = elems[1];
            }
        }
    }
    return true;
}

bool IniFile::save()
{
    return save(filename);
}

bool IniFile::save(const std::string &file)
{
    std::ofstream f(file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;

    uint32_t tmp = 0xbfbbef;
    bool first = false;
    f.write((char*)&tmp, 3); // BOM

    for(std::map<sf::String, std::map<sf::String, sf::String> >::iterator it = datas.begin(); it != datas.end(); ++it)
    {
        if(!it->first.isEmpty())
        {
            if(first)
            {
                #ifdef WINDOWS_VERSION
                f.write("\r", 1);
                #endif
                f.write("\n", 1);
            }
            first = true;
            mlib::writeUTF8char(f, '[');
            for(uint32_t i = 0; i < it->first.getSize(); ++i)
                mlib::writeUTF8char(f, it->first[i]);
            mlib::writeUTF8char(f, ']');
            #ifdef WINDOWS_VERSION
            f.write("\r", 1);
            #endif
            f.write("\n", 1);
        }

        for(std::map<sf::String, sf::String>::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
        {
            if(!jt->first.isEmpty())
            {
                for(uint32_t i = 0; i < jt->first.getSize(); ++i)
                    mlib::writeUTF8char(f, jt->first[i]);
                f.write("=", 1);
                for(uint32_t i = 0; i < jt->second.getSize(); ++i)
                    mlib::writeUTF8char(f, jt->second[i]);
                #ifdef WINDOWS_VERSION
                f.write("\r", 1);
                #endif
                f.write("\n", 1);
            }
        }
    }
    return true;
}
#endif
