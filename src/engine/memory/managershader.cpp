#include "managershader.hpp"
#ifdef _USE_SHADER_
#include "../engine.hpp"

ManagerShader::ManagerShader()
{
    //ctor
}

ManagerShader::~ManagerShader()
{
    for(auto &i : data) delete i.second;
}

bool ManagerShader::exist(const std::string &name)
{
    return (data.find(name) != data.end());
}

sf::Shader* ManagerShader::get(const std::string &name)
{
    std::map<std::string, sf::Shader*>::iterator it = data.find(name);
    if(it == data.end()) return NULL;
    return it->second;
}

void ManagerShader::erase(const std::string &name)
{
    std::map<std::string, sf::Shader*>::iterator it = data.find(name);
    if(it != data.end())
    {
        delete it->second;
        data.erase(it);
    }
}

sf::Shader* ManagerShader::create(const std::string &name, const std::string &vert, const std::string &frag)
{
    erase(name);
    sf::Shader *ptr = new sf::Shader();

    if(!vert.empty()) ptr->loadFromMemory(vert, sf::Shader::Vertex);
    if(!frag.empty()) ptr->loadFromMemory(frag, sf::Shader::Fragment);

    data[name] = ptr;
    return ptr;
}

void ManagerShader::clear()
{
    for(auto &i : data) delete i.second;
    data.clear();
}

std::string ManagerShader::loadShaderFile(const std::string &name)
{
    std::stringstream ss;
    std::ifstream f;
    std::istream *ptr = NULL;

    #ifdef _USE_FILESYSTEM_
    if(engine.files.loadFile(name))
    {
        engine.files.getStringStream(name,ss);
        ptr = (std::istream*)&ss;
    }
    else
    #endif
    {
        f.open(name.c_str(), std::ios::in | std::ios::binary);
        if(!f)
        {
            return "";
        }
        ptr = (std::istream*)&f;
    }

    std::string content;
    char c;
    while(ptr->good())
    {
        ptr->read(&c, 1);
        if(ptr->good()) content += c;
    }
    return content;
}

#endif
