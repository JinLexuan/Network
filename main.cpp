#include "Archive/Endpoint.h"

#include <boost/version.hpp>
#include <iostream>

int main(int argc, char const* argv[])
{
    (void) argc;
    (void) argv;

    np::Endpoint::createClientEndpoint();
    std::cout << BOOST_VERSION << std::endl;
    return 0;
}
