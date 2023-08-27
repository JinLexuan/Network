#include "Utils.h"

bool np::Utils::isBigEndian()
{
    const int num = 1;
    return *(char*) &num == 1 ? false : true; // Take out the data of first byte, if it is 1, then the data format is small endian.
}
