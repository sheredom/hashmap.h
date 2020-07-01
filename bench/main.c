// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

#include "ubench.h"
#include "hashmap.h"

UBENCH(create, initial_size_1_byte) {
  struct hashmap_s hashmap;
  hashmap_create(1, &hashmap);
  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

UBENCH(create, initial_size_1_kilobyte) {
  struct hashmap_s hashmap;
  hashmap_create(1024, &hashmap);
  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

struct put_small_keys {
  char *keys;
  unsigned key_len;
};

UBENCH_F_SETUP(put_small_keys) {
  const unsigned max_keys = 1024 * 1024;
  const unsigned key_len = 8;
  char *const keys = malloc(max_keys * key_len);
  unsigned i;

  for (i = 0; i < max_keys; i++) {
    snprintf(keys + (i * key_len), key_len, "%08x", i);
  }

  ubench_fixture->keys = keys;
  ubench_fixture->key_len = key_len;
}

UBENCH_F_TEARDOWN(put_small_keys) { free(ubench_fixture->keys); }

UBENCH_F(put_small_keys, 1) {
  struct hashmap_s hashmap;
  hashmap_create(1, &hashmap);
  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

UBENCH_F(put_small_keys, 1024) {
  struct hashmap_s hashmap;
  int i;

  hashmap_create(1, &hashmap);

  for (i = 0; i < 1024; i++) {
    const unsigned offset = i * ubench_fixture->key_len;
    hashmap_put(&hashmap, ubench_fixture->keys + offset,
                ubench_fixture->key_len, 0);
  }

  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

UBENCH_F(put_small_keys, 1048576) {
  struct hashmap_s hashmap;
  int i;

  hashmap_create(1, &hashmap);

  for (i = 0; i < 1048576; i++) {
    const unsigned offset = i * ubench_fixture->key_len;
    hashmap_put(&hashmap, ubench_fixture->keys + offset,
                ubench_fixture->key_len, 0);
  }

  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

struct put_large_keys {
  char *keys;
  unsigned key_len;
};

UBENCH_F_SETUP(put_large_keys) {
  const unsigned max_keys = 1024 * 1024;
  const unsigned key_len = 1024;
  char *const keys = malloc(max_keys * key_len);
  unsigned i;

  for (i = 0; i < max_keys; i++) {
    snprintf(keys + (i * key_len), key_len, "%01024x", i);
  }

  ubench_fixture->keys = keys;
  ubench_fixture->key_len = key_len;
}

UBENCH_F_TEARDOWN(put_large_keys) { free(ubench_fixture->keys); }

UBENCH_F(put_large_keys, 1) {
  struct hashmap_s hashmap;
  hashmap_create(1, &hashmap);
  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

UBENCH_F(put_large_keys, 1024) {
  struct hashmap_s hashmap;
  int i;

  hashmap_create(1, &hashmap);

  for (i = 0; i < 1024; i++) {
    const unsigned offset = i * ubench_fixture->key_len;
    hashmap_put(&hashmap, ubench_fixture->keys + offset,
                ubench_fixture->key_len, 0);
  }

  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

UBENCH_F(put_large_keys, 1048576) {
  struct hashmap_s hashmap;
  int i;

  hashmap_create(1, &hashmap);

  for (i = 0; i < 1048576; i++) {
    const unsigned offset = i * ubench_fixture->key_len;
    hashmap_put(&hashmap, ubench_fixture->keys + offset,
                ubench_fixture->key_len, 0);
  }

  UBENCH_DO_NOTHING(&hashmap);
  hashmap_destroy(&hashmap);
}

struct get_small_keys {
  char *keys;
  unsigned key_len;
  struct hashmap_s hashmap;
};

UBENCH_F_SETUP(get_small_keys) {
  const unsigned max_keys = 1024 * 1024;
  const unsigned key_len = 8;
  char *const keys = malloc(max_keys * key_len);
  unsigned i;
  struct hashmap_s hashmap;

  for (i = 0; i < max_keys; i++) {
    snprintf(keys + (i * key_len), key_len, "%08x", i);
  }
  hashmap_create(1, &hashmap);

  for (i = 0; i < max_keys; i++) {
    const unsigned offset = i * key_len;
    hashmap_put(&hashmap, keys + offset, key_len, 0);
  }

  ubench_fixture->keys = keys;
  ubench_fixture->key_len = key_len;
  ubench_fixture->hashmap = hashmap;
}

UBENCH_F_TEARDOWN(get_small_keys) {
  hashmap_destroy(&ubench_fixture->hashmap);
  free(ubench_fixture->keys);
}

UBENCH_F(get_small_keys, small_missing_key) {
  UBENCH_DO_NOTHING(hashmap_get(&ubench_fixture->hashmap, "a", strlen("a")));
}

UBENCH_F(get_small_keys, large_missing_key) {
#define DATA_SIZE (16 * 1024)
  char data[DATA_SIZE];
  const unsigned data_size = DATA_SIZE;
#undef DATA_SIZE
  unsigned i;

  for (i = 0; i < data_size; i++) {
    data[i] = (char)i;
  }

  UBENCH_DO_NOTHING(hashmap_get(&ubench_fixture->hashmap, data, data_size));
}

struct get_large_keys {
  char *keys;
  unsigned key_len;
  struct hashmap_s hashmap;
};

UBENCH_F_SETUP(get_large_keys) {
  const unsigned max_keys = 1024 * 1024;
  const unsigned key_len = 1024;
  char *const keys = malloc(max_keys * key_len);
  unsigned i;
  struct hashmap_s hashmap;

  for (i = 0; i < max_keys; i++) {
    snprintf(keys + (i * key_len), key_len, "%01024x", i);
  }
  hashmap_create(1, &hashmap);

  for (i = 0; i < max_keys; i++) {
    const unsigned offset = i * key_len;
    hashmap_put(&hashmap, keys + offset, key_len, 0);
  }

  ubench_fixture->keys = keys;
  ubench_fixture->key_len = key_len;
  ubench_fixture->hashmap = hashmap;
}

UBENCH_F_TEARDOWN(get_large_keys) {
  hashmap_destroy(&ubench_fixture->hashmap);
  free(ubench_fixture->keys);
}

UBENCH_F(get_large_keys, small_missing_key) {
  UBENCH_DO_NOTHING(hashmap_get(&ubench_fixture->hashmap, "a", strlen("a")));
}

UBENCH_F(get_large_keys, large_missing_key) {
#define DATA_SIZE (16 * 1024)
  char data[DATA_SIZE];
  const unsigned data_size = DATA_SIZE;
#undef DATA_SIZE
  unsigned i;

  for (i = 0; i < data_size; i++) {
    data[i] = (char)i;
  }

  UBENCH_DO_NOTHING(hashmap_get(&ubench_fixture->hashmap, data, data_size));
}

UBENCH_MAIN()
