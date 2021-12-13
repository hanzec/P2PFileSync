//
// Created by hanzech on 12/1/21.
//

#ifndef P2P_FILE_SYNC_BITPOS_H
#define P2P_FILE_SYNC_BITPOS_H
#include <asm/types.h>

/**
 * Bit position operations.
 *  ref from:
 *      https://github.com/torvalds/linux/blob/master/include/asm-generic/bitops/fls.h
 *      https://github.com/torvalds/linux/blob/master/include/asm-generic/bitops/fls64.h
 */
namespace P2PFileSync::bitops {

/**
 * fls - find last (most-significant) bit set
 * @x: the word to search
 *
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
constexpr static __always_inline int fls(uint32_t x) {
  int r = 32;

  if (!x) return 0;
  if (!(x & 0xffff0000u)) {
    x <<= 16;
    r -= 16;
  }
  if (!(x & 0xff000000u)) {
    x <<= 8;
    r -= 8;
  }
  if (!(x & 0xf0000000u)) {
    x <<= 4;
    r -= 4;
  }
  if (!(x & 0xc0000000u)) {
    x <<= 2;
    r -= 2;
  }
  if (!(x & 0x80000000u)) {
    x <<= 1;
    r -= 1;
  }
  return r;
}

/**
 * fls64 - find last set bit in a 64-bit word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffsll, but returns the position of the most significant set bit.
 *
 * fls64(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 64.
 */

constexpr static __always_inline int fls64(uint64_t x){
    __u32 h = x >> 32;
    if (h)
      return fls(h) + 32;
    return fls(static_cast<uint32_t>(x));
}

}  // namespace P2PFileSync::bitops
#endif  // P2P_FILE_SYNC_BITPOS_H
