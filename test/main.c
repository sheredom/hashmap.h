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

#include "utest.h"
#include "hashmap.h"

UTEST_MAIN()

UTEST(main, one_byte) {
  unsigned char data[256];
  int i;
  struct hashmap_s hashmap;

  for (i = 0; i < 256; i++) {
    data[i] = (unsigned char)i;
  }

  ASSERT_EQ(0, hashmap_create(1, &hashmap));

  for (i = 0; i < 256; i++) {
    ASSERT_EQ(0, hashmap_put(&hashmap, &data[i], 1, NULL));
  }

  ASSERT_EQ(hashmap_num_entries(&hashmap), 256u);
  ASSERT_LE(hashmap_capacity(&hashmap), 2048u);

  hashmap_destroy(&hashmap);
}

UTEST(main, two_bytes) {
  unsigned short *data;
  int i;
  struct hashmap_s hashmap;

  data = (unsigned short *)malloc(sizeof(unsigned short) * 16384);

  for (i = 0; i < 16384; i++) {
    data[i] = (unsigned short)i;
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
