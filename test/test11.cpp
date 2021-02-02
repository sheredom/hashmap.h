#include "hashmap.h"
#include "utest.h"

#if defined(_MSC_VER) && (_MSC_VER < 1920)
#define NOTHROW __declspec(nothrow)
#define NOEXCEPT
#else
#define NOTHROW
#define NOEXCEPT noexcept
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

UTEST(cpp11, create) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  hashmap_destroy(&hashmap);
}

UTEST(cpp11, create_zero) {
  struct hashmap_s hashmap;
  ASSERT_EQ(1, hashmap_create(0, &hashmap));
}

UTEST(cpp11, create_not_power_of_two) {
  struct hashmap_s hashmap;
  ASSERT_EQ(1, hashmap_create(3, &hashmap));
}

static int NOTHROW set_context(void *const context,
                               void *const element) NOEXCEPT {
  *static_cast<int *>(context) = *static_cast<int *>(element);
  return 1;
}

UTEST(cpp11, put) {
  struct hashmap_s hashmap;
  int x = 42;
  int y = 13;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           static_cast<unsigned>(strlen("foo")), &x));
  ASSERT_EQ(0, hashmap_iterate(&hashmap, set_context, &y));
  ASSERT_EQ(x, y);
  hashmap_destroy(&hashmap);
}

UTEST(cpp11, get_exists) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           static_cast<unsigned>(strlen("foo")), &x));
  ASSERT_EQ(&x, static_cast<int *>(hashmap_get(
                    &hashmap, "foo", static_cast<unsigned>(strlen("foo")))));
  hashmap_destroy(&hashmap);
}

UTEST(cpp11, get_does_not_exists) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_FALSE(
      hashmap_get(&hashmap, "foo", static_cast<unsigned>(strlen("foo"))));
  hashmap_destroy(&hashmap);
}

UTEST(cpp11, remove) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           static_cast<unsigned>(strlen("foo")), &x));
  ASSERT_EQ(
      0, hashmap_remove(&hashmap, "foo", static_cast<unsigned>(strlen("foo"))));
  hashmap_destroy(&hashmap);
}

static int NOTHROW early_exit(void *const context,
                              void *const element) NOEXCEPT {
  *static_cast<int *>(context) += 1;
  *static_cast<int *>(element) += 1;
  return 0;
}

UTEST(cpp11, iterate_early_exit) {
  struct hashmap_s hashmap;
  int x[27] = {0};
  int total = 0;
  char s[27];
  char c;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));

  for (c = 'a'; c <= 'z'; c++) {
    s[c - 'a'] = c;
  }

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(0, hashmap_put(&hashmap, s + index, 1, x + index));
  }

  ASSERT_EQ(1, hashmap_iterate(&hashmap, early_exit, &total));
  ASSERT_EQ(1, total);

  total = 0;

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_GE(1, x[index]);
    if (x[index]) {
      total += 1;
    }
  }

  ASSERT_EQ(1, total);

  hashmap_destroy(&hashmap);
}

static int NOTHROW all(void *const context, void *const element) NOEXCEPT {
  *static_cast<int *>(context) += 1;
  *static_cast<int *>(element) += 1;
  return 1;
}

UTEST(cpp11, iterate_all) {
  struct hashmap_s hashmap;
  int x[27] = {0};
  int total = 0;
  char s[27];
  char c;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));

  for (c = 'a'; c <= 'z'; c++) {
    s[c - 'a'] = c;
  }

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(0, hashmap_put(&hashmap, s + index, 1, x + index));
  }

  ASSERT_EQ(0, hashmap_iterate(&hashmap, all, &total));
  ASSERT_EQ(26, total);

  for (c = 'a'; c <= 'z'; c++) {
    const int index = c - 'a';
    ASSERT_EQ(1, x[index]);
  }

  hashmap_destroy(&hashmap);
}

UTEST(cpp11, num_entries) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           static_cast<unsigned>(strlen("foo")), &x));
  ASSERT_EQ(1u, hashmap_num_entries(&hashmap));
  ASSERT_EQ(
      0, hashmap_remove(&hashmap, "foo", static_cast<unsigned>(strlen("foo"))));
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap));
  hashmap_destroy(&hashmap);
}

UTEST(cpp11, hash_conflict) {
  struct hashmap_s hashmap;

  int x = 42;
  int y = 13;
  int z = -53;

  ASSERT_EQ(0, hashmap_create(4, &hashmap));

  // These all hash to the same value.
  ASSERT_EQ(0, hashmap_put(&hashmap, "000", 3, &x));
  ASSERT_EQ(0, hashmap_put(&hashmap, "002", 3, &y));
  ASSERT_EQ(0, hashmap_put(&hashmap, "007", 3, &z));
  ASSERT_EQ(3u, hashmap_num_entries(&hashmap));

  // Now we remove the middle value.
  ASSERT_EQ(0, hashmap_remove(&hashmap, "002", 3));
  ASSERT_EQ(2u, hashmap_num_entries(&hashmap));

  // And now attempt to insert the last value again. There was a bug where this
  // would insert a new entry incorrectly instead of resolving to the previous
  // entry.
  ASSERT_EQ(0, hashmap_put(&hashmap, "007", 3, &z));
  ASSERT_EQ(2u, hashmap_num_entries(&hashmap));

  hashmap_destroy(&hashmap);
}
