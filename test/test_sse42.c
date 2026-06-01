#include "hashmap.h"
#include "utest.h"

#if (defined(_MSC_VER) && defined(__AVX__)) || \
    (!defined(_MSC_VER) && defined(__SSE4_2__))

#define HASHMAP_SUITE c_sse42
#include "test_shared.h"

#endif
