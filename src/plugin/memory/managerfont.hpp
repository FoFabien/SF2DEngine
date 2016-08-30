#define _USE_FONT_
#ifdef _USE_FONT_
#ifndef MANAGERFONT_HPP
#define MANAGERFONT_HPP

#include <map>
#include <stdint.h>
#include "../file/filesystem.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/MemoryInputStream.hpp>

class ManagerFont
{
    public:
        ManagerFont();
        ~ManagerFont();
        bool exist(const std::string &name); // check if the file exist in memory
        const sf::Font* use(const std::string &name); // load the file if not found in memory and return a pointer. Increase its nUse value by 1.
        void trash(const std::string &name); // if the file is in memory, reduce its nUse value by 1.
        void trash(const sf::Font* ptr);
        void garbageCollection(); // remove the fonts not used (nUse <= 0)
        void setCurrentFont(const std::string &name);
        const sf::Font* useCurrentFont();
        void trashCurrentFont();
        void clear();
    protected:
        struct FontContainer
        {
            int32_t nUse;
            sf::Font *ptr;
            sf::MemoryInputStream stream_data;
        };
        std::map<std::string, FontContainer*> data;
        std::string currentFont;
};

#endif // MANAGERFONT_HPP
#endif
