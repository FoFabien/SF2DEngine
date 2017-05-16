#define _USE_SHADER_
#ifdef _USE_SHADER_
#ifndef MANAGERSHADER_HPP
#define MANAGERSHADER_HPP

#include <string>
#include "SFML/Graphics/Shader.hpp"

class ManagerShader
{
    public:
        ManagerShader();
        ~ManagerShader();
        bool exist(const std::string &name); // check if the shader exist in memory
        sf::Shader* get(const std::string &name); // return the shader pointer if found in memory. Increase its nUse value by 1
        void erase(const std::string &name); // decrease the shader nUse value by 1.
        sf::Shader* create(const std::string &name, const std::string &vert, const std::string &frag);
        void clear();
        static std::string loadShaderFile(const std::string &name);
    protected:
        std::map<std::string, sf::Shader*> data;
};

#endif // MANAGERSHADER_HPP
#endif
