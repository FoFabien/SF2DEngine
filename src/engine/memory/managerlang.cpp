#include "managerlang.hpp"
#ifdef _USE_LANG_
#include "../mlib/mlib.hpp"

ManagerLang::ManagerLang()
{
    current_lang_id = -1;
    folder = "";
}

ManagerLang::~ManagerLang()
{
    //dtor
}

void ManagerLang::setFolder(const sf::String &string)
{
    folder = string;
    if(folder[folder.size()-1] != '/') folder += '/';
}

size_t ManagerLang::getLangCount()
{
    return langs.size();
}

bool ManagerLang::getLang(ManagerLang_language &language, int32_t id)
{
    if(id < 0 || id >= (int32_t)langs.size()) return false;
    language = langs[id];
    return true;
}

bool ManagerLang::loadLang(int32_t id)
{
    if(id < 0 || id >= (int32_t)langs.size()) return false;
    if(id == current_lang_id) return true;
    Lang tmp;
    if(!tmp.loadFromMemory(folder + langs[id].file)) return false;
    current_lang_id = id;
    current_lang = tmp;
    return true;
}

bool ManagerLang::loadLang(sf::String name)
{
    int32_t id;
    for(id = 0; id < (int32_t)langs.size() && langs[id].name != name; ++id);
    if(id == (int32_t)langs.size()) return false;
    return loadLang(id);
}

sf::String ManagerLang::getString(const sf::String &string)
{
    return current_lang.getString(string);
}

bool ManagerLang::load(std::istream &f)
{
    langs.clear();
    uint32_t tmp;
    do
    {
        sf::String str;
        do
        {
            tmp = mlib::readUTF8char(f);
            if(tmp != 0x0000000a /* \n */&& tmp != 0x0000000d /* \d */&& f.good()) str += sf::String(tmp);
        }while(tmp != 0x0000000a /* \n */&& tmp != 0x0000000d /* \d */&& f.good());

        std::vector<sf::String> elems;
        mlib::sfsplit(str, {'|'}, elems);
        if(elems.size() >= 3)
        {
            ManagerLang_language language;
            language.name = elems[0];
            language.ts_name = elems[1];
            language.file = elems[2];
            langs.push_back(language);
        }
    }while(f.good());

    return true;
}

sf::String ManagerLang::getCurrentLangString()
{
    if(current_lang_id < 0 || current_lang_id >= (int32_t)langs.size()) return sf::String("");
    return langs[current_lang_id].name;
}

int32_t ManagerLang::getCurrentLang()
{
    return current_lang_id;
}

sf::String ManagerLang::getLangName(int32_t id)
{
    if(id < 0 || id >= (int32_t)langs.size()) return sf::String("");
    return langs[id].ts_name;
}

void ManagerLang::clear()
{
    current_lang_id = -1;
    folder = "";
    current_lang.clear();
    langs.clear();
}
#endif
