#include <boost/version.hpp>
#include <iostream>

int main(int argc, char const* argv[])
{
    (void) argc;
    (void) argv;

    std::cout << BOOST_VERSION << std::endl;
    return 0;
}
