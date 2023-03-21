# #️⃣ hashmap.h

[![Actions Status](https://github.com/sheredom/hashmap.h/workflows/CMake/badge.svg)](https://github.com/sheredom/hashmap.h/actions)
[![Build status](https://ci.appveyor.com/api/projects/status/1crw9uccf869aiss?svg=true)](https://ci.appveyor.com/project/sheredom/hashmap-h)
[![Sponsor](https://img.shields.io/badge/💜-sponsor-blueviolet)](https://github.com/sponsors/sheredom)

A simple one header hashmap implementation for C/C++.

## Usage

Just `#include "hashmap.h"` in your code!

The current supported compilers are gcc, clang and msvc.

The current supported platforms are Linux, macOS and Windows.

### Fundamental Design

The hashmap is made to work with any arbitrary data keys - you just provide a
pointer and size, and it'll hash that data. The default hasher is a crc32
variant using hardware intrinsics if possible, and the default comparer just
uses `memcmp`, so zeroing out any padding in struct keys is advisable.

### Create a Hashmap

To create a hashmap call the `hashmap_create` function:

```c
const unsigned initial_size = 2;
struct hashmap_s hashmap;
if (0 != hashmap_create(initial_size, &hashmap)) {
  // error!
}
```

The `initial_size` parameter only sets the initial size of the hashmap - which
can grow if multiple keys hit the same hash entry. The size of the hashmap is
rounded up to the nearest power of two above the provided `initial_size`.

There is also an extended creation function `hashmap_create_ex`:

```c
struct hashmap_s hashmap;
struct hashmap_create_options_s options;
memset(&options, 0, sizeof(options));

// You can set a custom hasher that the hashmap should use.
options.hasher = &my_hasher;

// You can set a custom comparer that the hashmap should for comparing keys.
options.comparer = &my_comparer;

// You can also specify the initial capacity of the hashmap.
options.initial_capacity = 42;

if (0 != hashmap_create_ex(options, &hashmap)) {
  // error!
}
```

### Put Something in a Hashmap

To put an item in the hashmap use the `hashmap_put` function:

```c
int meaning_of_life = 42;
char question = 6 * 8;

if (0 != hashmap_put(&hashmap, "life", strlen("life"), &meaning_of_life)) {
  // error!
}

if (0 != hashmap_put(&hashmap, "?", strlen("?"), &question)) {
  // error!
}
```

Notice that multiple entries of _differing_ types can exist in the same hashmap.
The hashmap is not typed - it can store any `void*` data as the value for a key.

### Get Something from a Hashmap

To get an entry from a hashmap use the `hashmap_get` function:

```c
void* const element = hashmap_get(&hashmap, "x", strlen("x"));
if (NULL == element) {
  // error!
}
```

The function will return `NULL` if the element is not found. Note that the key
used to get an element does not have to be the same pointer used to put an
element in the hashmap - but the string slice must match for a hit to occur.

### Remove Something from a Hashmap

To remove an entry from a hashmap use the `hashmap_remove` function:

```c
if (0 != hashmap_remove(&hashmap, "x", strlen("x"))) {
  // error!
}
```

The function will return non-zero if the element is not found. Note that the key
used to get an element does not have to be the same pointer used to put an
element in the hashmap - but the string slice must match for a hit to occur.

### Iterate Over a Hashmap

You can iterate over all the elements stored in the hashmap with the
`hashmap_iterate` function:

```c
static int iterate(void* const context, void* const value) {
  // If the value is 42...
  if (42 == *(int*)value) {
    // Store into our user-provided context the value.
    *(void**) context = value;

    // Return 0 to tell the iteration to stop here.
    return 0;
  }

  // Otherwise tell the iteration to keep going.
  return 1;
}

int* value;
if (0 != hashmap_iterate(&hashmap, iterate, &value)) {
  if (*value != 42) {
    // error!
  }
} else {
  // if we got here it means we never found 42 in the hashmap
}
```

You can early exit from the iteration of the hashmap by returning non-zero from
your callback function - perhaps you want to process and remove all elements
from the hashmap or search for a specific value only. Otherwise if zero is
returned from your callback then the iteration will encompass the entire
hashmap.

### Alternate Iterator for Key Value Pairs

In some applications, such as needing to print out the contents of a hashmap,
you need to have access to the key and key length in addition to the value.
For that purpose a second iterator has been added called
`hashmap_iterate_pairs`.

Also, returning a -1 from the callback function allows automatic removal of the
current item.  This is especially handy when storing dynamically allocated
objects to the map and needing to free the memory when destroying the map.

```c
int log_and_free_all(void* const context, struct hashmap_element_s* const e) {
  int counter;
  for (counter=0; counter < e->key_len; counter++) {
    fputc(e->key[counter], (FILE)context);
  }
  fprintf((FILE)context,"=%s pair has been freed\n", (char *)e->data);
  free(e->data);
  return -1;
}

void shut_down() {
  if (0!=hashmap_iterate_pairs(&hash, log_and_free_all, (void *)log)) {
    fprintf(stderr, "failed to deallocate hashmap entries\n");
  }
  fclose(log);
  hashmap_destroy(&hash);
}
```

### Get the Number of Entries in a Hashmap

To get the number of entries that have been put into a hashmap use the
`hashmap_num_entries` function:

```c
unsigned num_entries = hashmap_num_entries(&hashmap);
```

### Get the Capcity of a Hashmap

To get the actual number of buckets allocated in the hashmap (the capacity) use
the `hashmap_capacity` function:

```c
unsigned num_entries = hashmap_capacity(&hashmap);
```

### Destroy a Hashmap

To destroy a hashmap when you are finished with it use the `hashmap_destroy`
function:

```c
hashmap_destroy(&hashmap);
```

## Code Ownership

This code was almost entirely written by the awesome
[Pete Warden](https://twitter.com/petewarden), based on a now defunct
[blog post](https://web.archive.org/web/20160329102146/http://elliottback.com/wp/hashmap-implementation-in-c/)
by Elliott Back. The authors have applied the following further changes:

- Merged the .c / .h to create a single header (meaning easier integrations with
  external projects).
- Used an explicitly public domain license for the code - the
  [unlicense](https://unlicense.org/).
- Changed the API to take arbitrary data pointers and length (it was originally
  solely for UTF-8 string slices).
- Did a pass to clean up the comments and function signatures.
- Added second iterator, tests and documentation. (Samuel D. Crow)

## License

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
