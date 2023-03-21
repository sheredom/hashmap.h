#if defined(__cplusplus) && (__cplusplus >= 201103L) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#if defined(__cplusplus) && defined(_MSC_VER) && (_MSC_VER < 1920)
#define NOTHROW __declspec(nothrow)
#define NOEXCEPT
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
#define NOTHROW
#define NOEXCEPT noexcept
#else
#define NOTHROW
#define NOEXCEPT
#endif

#include <string.h>

MY_TEST_WRAPPER(create) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(create_zero) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(0, &hashmap));
  ASSERT_LT(0u, hashmap_capacity(&hashmap));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(create_not_power_of_two) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(3, &hashmap));
  ASSERT_LE(3u, hashmap_capacity(&hashmap));
  hashmap_destroy(&hashmap);
}

static hashmap_uint32_t custom_hasher(const hashmap_uint32_t seed,
                                      const void *const s,
                                      const hashmap_uint32_t len) {
  hashmap_uint64_t cs = 0;
  memcpy(&cs, &s, sizeof(s));

  return seed ^ HASHMAP_CAST(hashmap_uint32_t, cs >> 32) ^
         HASHMAP_CAST(hashmap_uint32_t, cs) ^ len;
}

static int custom_comparer(const void *const a, const hashmap_uint32_t a_len,
                           const void *const b, const hashmap_uint32_t b_len) {
  return (a_len == b_len) &&
         0 == strncmp(HASHMAP_PTR_CAST(const char *, a),
                      HASHMAP_PTR_CAST(const char *, b), a_len);
}

MY_TEST_WRAPPER(create_ex) {
  int thing = 42;
  const char *const key = "foo";
  struct hashmap_s hashmap;
  struct hashmap_create_options_s options;
  memset(&options, 0, sizeof(options));
  options.initial_capacity = 42;
  options.hasher = &custom_hasher;
  options.comparer = &custom_comparer;

  ASSERT_EQ(0, hashmap_create_ex(options, &hashmap));
  ASSERT_LE(42u, hashmap_capacity(&hashmap));

  ASSERT_EQ(0, hashmap_put(&hashmap, key, HASHMAP_CAST(unsigned, strlen(key)),
                           &thing));

  ASSERT_EQ(&thing,
            HASHMAP_PTR_CAST(const int *,
                             hashmap_get(&hashmap, key,
                                         HASHMAP_CAST(unsigned, strlen(key)))));

  ASSERT_EQ(1u, hashmap_num_entries(&hashmap));

  hashmap_destroy(&hashmap);
}

static int NOTHROW set_context(void *const context,
                               void *const element) NOEXCEPT {
  *HASHMAP_PTR_CAST(int *, context) = *HASHMAP_PTR_CAST(int *, element);
  return 1;
}

MY_TEST_WRAPPER(put) {
  struct hashmap_s hashmap;
  int x = 42;
  int y = 13;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           HASHMAP_CAST(unsigned, strlen("foo")), &x));
  ASSERT_EQ(0, hashmap_iterate(&hashmap, set_context, &y));
  ASSERT_EQ(x, y);
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(get_exists) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           HASHMAP_CAST(unsigned, strlen("foo")), &x));
  ASSERT_EQ(&x, HASHMAP_PTR_CAST(
                    int *, hashmap_get(&hashmap, "foo",
                                       HASHMAP_CAST(unsigned, strlen("foo")))));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(get_does_not_exists) {
  struct hashmap_s hashmap;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_FALSE(
      hashmap_get(&hashmap, "foo", HASHMAP_CAST(unsigned, strlen("foo"))));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(remove) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           HASHMAP_CAST(unsigned, strlen("foo")), &x));
  ASSERT_EQ(0, hashmap_remove(&hashmap, "foo",
                              HASHMAP_CAST(unsigned, strlen("foo"))));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(remove_and_return_key) {
  /* The '&bar' portion of the string just uniques the constant from the 'foo'
   * used later. */
  const char *const key = "foo&bar";

  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, key, 3, &x));

  /* Use a new string here so that we definitely have a different pointer key
   * being provided. */
  ASSERT_EQ(key, HASHMAP_PTR_CAST(const char *,
                                  hashmap_remove_and_return_key(
                                      &hashmap, "foo",
                                      HASHMAP_CAST(unsigned, strlen("foo")))));
  hashmap_destroy(&hashmap);
}

static int NOTHROW early_exit(void *const context, void *const element) {
  *HASHMAP_PTR_CAST(int *, context) += 1;
  *HASHMAP_PTR_CAST(int *, element) += 1;
  return 0;
}

MY_TEST_WRAPPER(iterate_early_exit) {
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

static int NOTHROW all(void *const context, void *const element) {
  *HASHMAP_PTR_CAST(int *, context) += 1;
  *HASHMAP_PTR_CAST(int *, element) += 1;
  return 1;
}

MY_TEST_WRAPPER(iterate_all) {
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

MY_TEST_WRAPPER(num_entries) {
  struct hashmap_s hashmap;
  int x = 42;
  ASSERT_EQ(0, hashmap_create(1, &hashmap));
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap));
  ASSERT_EQ(0, hashmap_put(&hashmap, "foo",
                           HASHMAP_CAST(unsigned, strlen("foo")), &x));
  ASSERT_EQ(1u, hashmap_num_entries(&hashmap));
  ASSERT_EQ(0, hashmap_remove(&hashmap, "foo",
                              HASHMAP_CAST(unsigned, strlen("foo"))));
  ASSERT_EQ(0u, hashmap_num_entries(&hashmap));
  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(hash_conflict) {
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

MY_TEST_WRAPPER(issue_20) {
  struct hashmap_s hashmap;
  const char *key = "192.168.2.2hv_api.udache.com/abc/def";
  unsigned int len = HASHMAP_CAST(unsigned, strlen(key));

  int value = 42;
  int *ptr = UTEST_NULL;

  hashmap_create(1024, &hashmap);
  hashmap_put(&hashmap, key, len, &value);

  ptr = HASHMAP_PTR_CAST(int *, hashmap_get(&hashmap, key, len));

  ASSERT_EQ(&value, ptr);

  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(one_byte) {
  unsigned char data[256];
  int i;
  struct hashmap_s hashmap;

  for (i = 0; i < 256; i++) {
    data[i] = HASHMAP_CAST(unsigned char, i);
  }

  ASSERT_EQ(0, hashmap_create(1, &hashmap));

  for (i = 0; i < 256; i++) {
    ASSERT_EQ(0, hashmap_put(&hashmap, &data[i], 1, NULL));
  }

  ASSERT_EQ(hashmap_num_entries(&hashmap), 256u);
  ASSERT_LE(hashmap_capacity(&hashmap), 1024u);

  hashmap_destroy(&hashmap);
}

MY_TEST_WRAPPER(two_bytes) {
  unsigned short *data;
  int i;
  struct hashmap_s hashmap;

  data = HASHMAP_PTR_CAST(unsigned short *,
                          malloc(sizeof(unsigned short) * 16384));

  for (i = 0; i < 16384; i++) {
    data[i] = HASHMAP_CAST(unsigned short, i);
  }

  ASSERT_EQ(0, hashmap_create(1, &hashmap));

  for (i = 0; i < 16384; i++) {
    ASSERT_EQ(0, hashmap_put(&hashmap, &data[i], 2, NULL));
  }

  ASSERT_EQ(hashmap_num_entries(&hashmap), 16384u);
  ASSERT_LE(hashmap_capacity(&hashmap), 65536u);

  hashmap_destroy(&hashmap);
  free(data);
}
