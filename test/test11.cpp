#include "hashmap.h"
#include "utest.h"
#include "ubench.h"

#define MY_TEST_WRAPPER(name) UTEST(cpp11, name)
#define MY_BENCH_WRAPPER(name) UBENCH_EX(cpp11, name)

#include "test.inc"
