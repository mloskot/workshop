// Does CMake define NDEBUG in non-Debug builds?
#include "cmake_build_type.h"
#include <cstdio>

int main()
{
    // Possible values are empty, Debug, Release, RelWithDebInfo and MinSizeRel.
    // This variable is only supported for make based generators.
    std::printf("CMAKE_BUILD_TYPE=%s\n", CMAKE_BUILD_TYPE);
#ifdef NDEBUG
    std::printf("NDEBUG=%d\n", NDEBUG);
#else
    std::puts("NDEBUG not defined");
#endif
}
