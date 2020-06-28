# #️⃣ hashmap.h

[![Build status](https://ci.appveyor.com/api/projects/status/1crw9uccf869aiss?svg=true)](https://ci.appveyor.com/project/sheredom/hashmap-h)
[![Build Status](https://travis-ci.com/sheredom/hashmap.h.svg?branch=master)](https://travis-ci.com/sheredom/hashmap.h)

A simple one header hashmap implementation for C/C++.

## Usage

Just `#include "hashmap.h"` in your code!

The current supported compilers are gcc, clang and msvc.

The current supported platforms are Linux, macOS and Windows.

### Fundamental Design

The hashmap is made to work with UTF-8 string slices - sections of strings that
are passed with a pointer and an explicit length. The reason for this design
choice was that the hashmap is being used, by the author, to map symbols that
are already resident in memory from a source file of a programming language. To
save from causing millions of additional allocations to move these UTF-8 string
slices into null-terminated strings, an explicit length is always passed.

Note also that while the API passes char* pointers as the key - these keys are
never used with the C API string functions. Instead `memcmp` is used to compare
keys. This allows us to use UTF-8 strings in place of regular ASCII strings with
no additional code.

### Create a Hashmap

To create a hashmap call the `hashmap_create` function:

```c
const unsigned initial_size = 4;
struct hashmap_s hashmap;
if (0 != hashmap_create(initial_size, &hashmap)) {
  // error!
}
```

The `initial_size` parameter only sets the initial size of the hashmap - which
can grow if multiple keys hit the same hash entry.

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
    *(void**) = value;

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

### Get the Number of Entries in a Hashmap

To get the number of entries that have been put into a hashmap use the
`hashmap_num_entries` function:

```c
unsigned num_entries = hashmap_num_entries(&hashmap);
```

### Destroy a Hashmap

To destroy a hashmap when you are finished with it use the `hashmap_destroy`
function:

```c
hashmap_destroy(&hashmap);
```

## CRC32 Function

The implementation here was originally done by Gary S. Brown in 1986, who let
the code or tables extracted from it without restriction.

First, the polynomial itself and its table of feedback terms.  The polynomial is
`X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0`. Note that
we take it "backwards" and put the highest-order term in the lowest-order bit.
The `X^32` term is "implied"; the LSB is the `X^31` term, etc. The `X^0` term
(usually shown as "+1") results in the MSB being 1. Note that the usual hardware
shift register implementation, which is what we're using (we're merely
optimizing it by doing eight-bit chunks at a time) shifts bits into the
lowest-order term. In our implementation, that means shifting towards the right. 
Why do we do it this way? Because the calculated CRC must be transmitted in
order from highest-order term to lowest-order term. UARTs transmit characters in
order from LSB to MSB. By storing the CRC this way, we hand it to the UART in
the order low-byte to high-byte; the UART sends each low-bit to hight-bit; and
the result is transmission bit by bit from highest- to lowest-order term without
requiring any bit shuffling on our part. Reception works similarly. The feedback
terms table consists of 256, 32-bit entries. Notes: The table can be generated
at runtime if desired; code to do so is shown later. It might not be obvious,
but the feedback terms simply represent the results of eight shift/xor
operations for all combinations of data and CRC register values. The values must
be right-shifted by eight bits by the "updcrc" logic; the shift must be unsigned
(bring in zeroes). On some hardware you could probably optimize the shift in
assembler by using byte-swap instructions.

## Code Ownership

This code was almost entirely written by the awesome
[Pete Warden](https://twitter.com/petewarden), based on a now defunct
[blog post](https://web.archive.org/web/20160329102146/http://elliottback.com/wp/hashmap-implementation-in-c/)
by Elliott Back. The author has applied the following further changes:

- Merged the .c / .h to create a single header (meaning easier integrations with
  external projects).
- Used an explicitly public domain license for the code - the
  [unlicense](https://unlicense.org/).
- Changed the API to take string slices (pointer & length) instead of null
  terminated strings.
- Did a pass to clean up the comments and function signatures.

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
