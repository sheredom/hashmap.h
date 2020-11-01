#include "hashmap.h"
#include "utest.h"

#if (defined(_MSC_VER) && defined(__AVX__)) ||                                 \
    (!defined(_MSC_VER) && defined(__SSE4_2__))
UTEST(c_sse42, create) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  hashmap_destroy(&hashmap);
}

UTEST(c_sse42, create_zero) {
  struct hashmap_s hashmap;
  ASSERT_EQ(1, hashmap_create(0, &hashmap))
}

UTEST(c_sse42, create_not_power_of_two) {
  struct hashmap_s hashmap;
  ASSERT_EQ(1, hashmap_create(3, &hashmap))
}

static int set_context(void *const context, void *const element) {
  *(int *)context = *(int *)element;
  return 1;
}

UTEST(c_sse42, put) {
  struct hashmap_s hashmap;
  int x = 42;
  int y = 13;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo", (unsigned)strlen("foo"), &x))
  ASSERT_EQ(0, hashmap_put(&hashmap, "bar", (unsigned)strlen("bar"), &x))
  ASSERT_EQ(0, hashmap_iterate(&hashmap, set_context, &y))
  ASSERT_EQ(x, y)
  hashmap_destroy(&hashmap);
}

UTEST(c_sse42, get_exists) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo", (unsigned)strlen("foo"), &x))
  ASSERT_EQ(&x, hashmap_get(&hashmap, "foo", (unsigned)strlen("foo")))
  hashmap_destroy(&hashmap);
}

UTEST(c_sse42, get_does_not_exists) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  ASSERT_EQ(NULL, hashmap_get(&hashmap, "foo", (unsigned)strlen("foo")))
  hashmap_destroy(&hashmap);
}

UTEST(c_sse42, remove) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo", (unsigned)strlen("foo"), &x))
  ASSERT_EQ(0, hashmap_remove(&hashmap, "foo", (unsigned)strlen("foo")))
  hashmap_destroy(&hashmap);
}

static int early_exit(void *const context, void *const element) {
  *(int *)context += 1;
  *(int *)element += 1;
  return 0;
}

UTEST(c_sse42, iterate_early_exit) {
  struct hashmap_s hashmap;
  int x[27] = {0};
  int total = 0;
  char s[27];
  char c;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))

  for (c = 'a'; c <= 'z'; c++) {
    s[c - 'a'] = c;
  }

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(0, hashmap_put(&hashmap, s + index, 1, x + index))
  }

  ASSERT_EQ(1, hashmap_iterate(&hashmap, early_exit, &total))
  ASSERT_EQ(1, total)

  total = 0;

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_GE(1, x[index])
    if (x[index]) {
      total += 1;
    }
  }

  ASSERT_EQ(1, total)

  hashmap_destroy(&hashmap);
}

static int all(void *const context, void *const element) {
  *(int *)context += 1;
  *(int *)element += 1;
  return 1;
}

UTEST(c_sse42, iterate_all) {
  struct hashmap_s hashmap;
  int x[27] = {0};
  int total = 0;
  char s[27];
  char c;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))

  for (c = 'a'; c <= 'z'; c++) {
    s[c - 'a'] = c;
  }

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(0, hashmap_put(&hashmap, s + index, 1, x + index))
  }

  ASSERT_EQ(0, hashmap_iterate(&hashmap, all, &total))
  ASSERT_EQ(26, total)

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(1, x[index])
  }

  hashmap_destroy(&hashmap);
}

UTEST(c_sse42, num_entries) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap))
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap))
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo", (unsigned)strlen("foo"), &x))
  ASSERT_EQ(1u, hashmap_num_entries(&hashmap))
  ASSERT_EQ(0, hashmap_remove(&hashmap, "foo", (unsigned)strlen("foo")))
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap))
  hashmap_destroy(&hashmap);
}

extern unsigned crc32_scalar(const char *const s, const unsigned len);

UTEST(c_sse42, crc32_matches) {
  unsigned index;

  for (index = 0; index <= 0xff; index++) {
    const char c = (char)index;

    ASSERT_EQ(crc32_scalar(&c, 1), hashmap_crc32_helper(&c, 1))
  }
}

#endif
