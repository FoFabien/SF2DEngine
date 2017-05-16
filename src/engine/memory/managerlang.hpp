#ifndef MANAGERLANG_HPP
#define MANAGERLANG_HPP

#include "../mlib/mload.hpp"
#include "../file/lang.hpp"
#ifdef _USE_LANG_
#include <vector>
#include <stdint.h>

struct ManagerLang_language
{
    sf::String name;
    sf::String ts_name;
    std::string file;
};

class ManagerLang: public MLoad
{
    public:
        ManagerLang();
        ~ManagerLang();
        void setFolder(const sf::String &string);
        size_t getLangCount();
        bool getLang(ManagerLang_language &language, int32_t id);
        bool loadLang(int32_t id);
        bool loadLang(sf::String name);
        sf::String getString(const sf::String &string);
        sf::String getCurrentLangString();
        int32_t getCurrentLang();
        sf::String getLangName(int32_t id);
        void clear();
    protected:
        bool load(std::istream &f);

        Lang current_lang;
        int32_t current_lang_id;
        std::vector<ManagerLang_language> langs;
        std::string folder;
};
#endif

#endif // MANAGERLANG_HPP
