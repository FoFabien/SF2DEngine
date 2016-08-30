#include "lang.hpp"
#ifdef _USE_LANG_
#include "../../mlib/mlib.hpp"

Lang::Lang()
{
    //ctor
}

Lang::~Lang()
{
    //dtor
}

sf::String Lang::getString(const sf::String &string)
{
    if(data.find(string) == data.end()) return string;
    //return applyGlobalVariable(data[string]);
    return data[string];
}

void Lang::clear()
{
    data.clear();
}

bool Lang::load(std::istream &f)
{
    data.clear();
    sf::String s1, s2;
    bool state = false;
    uint32_t tmp;
    while(f.good())
    {
        tmp = mlib::readUTF8char(f);
        if(f.good())
        {
            switch(tmp)
            {
                case '=':
                    if(!state) state = true; // end of first term
                    else s2 += sf::String(tmp); // ignored
                    break;
                case '\n':
                    if(state) // end of line
                    {
                        pre_parse(s2);
                        data[s1] = s2;
                        s1.clear();
                        s2.clear();
                        state = false;
                    }
                    else // error
                    {
                        s1.clear();
                        s2.clear();
                    }
                    break;
                case '\r': case '\a': case '\f': case '\v': break; // ignored
                default:
                    if(!state) s1 += sf::String(tmp); // first term
                    else s2 += sf::String(tmp); // second term
                    break;
            }
        }
    }
    if(state) // end of line
    {
        pre_parse(s2);
        data[s1] = s2;
    }
    return true;
}

void Lang::pre_parse(sf::String &string)
{
    for(uint32_t i = 0; i < string.getSize() - 1; ++i)
    {
        switch(string[i])
        {
            case '\\':
            {
                switch(string[i+1])
                {
                    case 'n':
                    {
                        string[i] = '\n';
                        string.erase(i+1);
                        break;
                    }
                    default: break;
                }
                break;
            }
            default: break;
        }
    }
}
#endif
