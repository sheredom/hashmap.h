#include "hashmap.h"
#include "utest.h"
#include "ubench.h"

#define MY_TEST_WRAPPER(name) UTEST(cpp, name)
#define MY_BENCH_WRAPPER(name) UBENCH_EX(cpp, name)

#include "test.inc"
