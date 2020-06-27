# #️⃣ hashmap.h

[![Build status](https://ci.appveyor.com/api/projects/status/1crw9uccf869aiss?svg=true)](https://ci.appveyor.com/project/sheredom/hashmap-h)
[![Build Status](https://travis-ci.com/sheredom/hashmap.h.svg?branch=master)](https://travis-ci.com/sheredom/hashmap.h)

A simple one header hashmap implementation for C/C++.

## Usage

Just `#include "hashmap.h"` in your code!

The current supported compilers are gcc, clang and msvc.

The current supported platforms are Linux, macOS and Windows.

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

## Ownership

This code was almost entirely written by the awesome
[Pete Warden](https://twitter.com/petewarden), based on a now defunct
[blog post](https://web.archive.org/web/20160329102146/http://elliottback.com/wp/hashmap-implementation-in-c/)
by Elliott Back. The author has applied the following further changes:

- Merged the .c / .h to create a single header (meaning easier integrations in
  build systems).
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
