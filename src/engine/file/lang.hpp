#ifndef LANG_HPP
#define LANG_HPP
#define _USE_LANG_
#ifdef _USE_LANG_

#include <map>
#include <SFML/Window.hpp>
#include "../mlib/mload.hpp"

class Lang: public MLoad
{
    public:
        Lang();
        ~Lang();
        sf::String getString(const sf::String &string);
        void clear();
    protected:
        bool load(std::istream &f);
        void pre_parse(sf::String &string);

        std::map<sf::String, sf::String> data;
};

#endif
#endif // LANG_HPP
