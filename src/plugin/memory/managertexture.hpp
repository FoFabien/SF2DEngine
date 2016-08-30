#define _USE_TEXTURE_
#ifdef _USE_TEXTURE_
#ifndef MANAGERTEXTURE_HPP
#define MANAGERTEXTURE_HPP

#include <map>
#include <stdint.h>
#include <SFML/Graphics.hpp>

class ManagerTexture
{
    public:
        ManagerTexture();
        ~ManagerTexture();
        bool exist(const std::string &name); // check if the texture exist in memory
        sf::Texture* use(const std::string &name); // load the texture if not found in memory and return a pointer. Increase its nUse value by 1
        void trash(const std::string &name); // decrease the texture nUse value by 1.
        void trash(const sf::Texture* ptr); // decrease the texture nUse value by 1.
        void garbageCollection(); // delete all the textures not used (nUse <= 0)

        void clear();
    protected:
        struct TextureContainer
        {
            sf::Texture *ptr;
            int32_t nUse;
        };
        std::map<std::string, TextureContainer> data;
};

#endif // MANAGERTEXTURE_HPP
#endif
