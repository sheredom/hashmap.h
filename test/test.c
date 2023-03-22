#include "hashmap.h"
#include "utest.h"
#include "ubench.h"

#define MY_TEST_WRAPPER(name) UTEST(c, name)
#define MY_BENCH_WRAPPER(name) UBENCH_EX(c, name)

#include "test.inc"
