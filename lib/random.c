#include <fstream>
#include <boost/random.hpp>
#include "random.h"

int get_seed() {
    std::ifstream rand("/dev/urandom");
    char tmp[sizeof(int)];
    rand.read(tmp, sizeof(int));
    rand.close();
    int* number = reinterpret_cast<int*>(tmp);
    return (*number);
}

int random_gen(int floor, int ceil) {
    typedef boost::minstd_rand rng_type;
    typedef boost::uniform_smallint<int> dist_type;
    typedef boost::variate_generator<rng_type&, dist_type> generator_type;
    
    // distribution maps from floor to ceil
    dist_type dist(floor, ceil);
    rng_type generator;
    unsigned int seed = static_cast<unsigned int>(get_seed());
    generator.seed(seed);
    generator_type gen(generator, dist);
    return gen();
}
