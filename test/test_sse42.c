#include "hashmap.h"
#include "utest.h"

#if (defined(_MSC_VER) && defined(__AVX__)) ||                                 \
    (!defined(_MSC_VER) && defined(__SSE4_2__))

#define MY_TEST_WRAPPER(name) UTEST(c_sse2, name)

#include "test.inc"

#endif
