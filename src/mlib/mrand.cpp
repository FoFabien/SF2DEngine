#include "mrand.hpp"

namespace mrand
{
    static uint32_t seed;

    void setSeed(uint32_t s)
    {
        seed = s;
    }

    uint32_t getSeed()
    {
        return seed;
    }

    int32_t rand()
    {
        seed = seed * 1103515245 + 12345;
        return seed;
    }

    int32_t rand(int32_t min, int32_t max)
    {
        if(min > max)
        {
            int32_t tmp = max;
            max = min;
            min = tmp;
        }
        seed = seed * 1103515245 + 12345;
        return seed % (max - min + 1) + min;
    }

    int32_t rand(uint32_t max)
    {
        seed = seed * 1103515245 + 12345;
        return seed % (max + 1);
    }

    bool proba_percent(int32_t proba)
    {
        return (rand(1, 100) <= proba);
    }

    bool proba_permille(int32_t proba)
    {
        return (rand(1, 1000) <= proba);
    }

    bool boolean()
    {
        return (rand()%2 == 1);
    }
}
