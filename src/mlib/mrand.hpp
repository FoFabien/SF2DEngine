#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <stdint.h>

namespace mrand
{
    void setSeed(uint32_t s); // set the initial seed
    uint32_t getSeed(); // get the current seed

    int32_t rand();
    int32_t rand(int32_t min, int32_t max);
    int32_t rand(uint32_t max);

    bool proba_percent(int32_t proba); // do a rand(0, 100) and return true if the result is inferior or equal to proba
    bool proba_permille(int32_t proba); // do a rand(0, 1000) and return true if the result is inferior or equal to proba
    bool boolean(); // return true or false (50%)
}


#endif // RANDOM_HPP
