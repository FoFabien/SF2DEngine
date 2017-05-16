#ifndef INIFILE_HPP
#define INIFILE_HPP
#define _USE_INIFILE_
#ifdef _USE_INIFILE_

#include <map>
#include <stdint.h>
#include <SFML/System/String.hpp>

class IniFile
{
    public:
        IniFile();
        ~IniFile();
        void editStrData(const sf::String &name, const sf::String &data, const sf::String &category = "");
        void editBoolData(const sf::String &name, const bool &data, const sf::String &category = "");
        void editIntData(const sf::String &name, const int32_t &data, const sf::String &category = "");
        void editFloatData(const sf::String &name, const float &data, const sf::String &category = "");

        sf::String getStrData(const sf::String &name, const sf::String &category = "");
        bool getBoolData(const sf::String &name, const sf::String &category = "");
        int32_t getIntData(const sf::String &name, const sf::String &category = "");
        float getFloatData(const sf::String &name, const sf::String &category = "");

        bool exist(const sf::String &name, const sf::String &category = "");
        void erase(const sf::String &name, const sf::String &category = "");

        bool load(const std::string &file);
        bool save();
        bool save(const std::string &file);
    protected:
        std::string filename;
        std::map<sf::String, std::map<sf::String, sf::String> > datas;
};

#endif
#endif // INIFILE_HPP
