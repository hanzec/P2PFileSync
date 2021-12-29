//
// Created by hanzech on 12/1/21.
//

#include "rabin_checksum.h"

#include <strings.h>

#include <array>
#include "macro.h"
#include "bitpos.h"

#define INT64(n) n##LL
#define MSB64 INT64(0x8000000000000000)

namespace P2PFileSync::RabinChecksum {
/**
 * Static generator of the polynomial.
 */
constexpr uint64_t poly = 0xbfe6b8a5bf378d83LL;
constexpr ALWAYS_INLINE uint64_t append8(uint64_t p, unsigned char m,
                                           const std::array<uint64_t, 256> &T) {
  return ((p << 8) | m) ^ T.at((p >> int(bitops::fls64(poly) - 9)));
}

constexpr ALWAYS_INLINE u_int64_t polymod(uint64_t nh, uint64_t nl, uint64_t d) {
  int i = 0;
  int k = bitops::fls64(d) - 1;

  d <<= 63 - k;

  if (nh) {
    if (nh & MSB64) nh ^= d;
    for (i = 62; i >= 0; i--)
      if (nh & ((u_int64_t)1) << i) {
        nh ^= d >> (63 - i);
        nl ^= d << (i + 1);
      }
  }
  for (i = 63; i >= k; i--) {
    if (nl & INT64(1) << i) nl ^= d >> (63 - i);
  }

  return nl;
}

constexpr ALWAYS_INLINE u_int64_t polymmult(uint64_t x, uint64_t y, uint64_t d) {
  u_int64_t ph = 0, pl = 0;
  if (x & 1) pl = y;
  for (uint_fast8_t i = 1; i < 64; i++)
    if (x & (INT64(1) << i)) {
      ph ^= y >> (64 - i);
      pl ^= y << i;
    }
  return polymod(ph, pl, d);
}

static constexpr std::array<uint64_t, 256> calcT() {
  std::array<u_int64_t, 256> ret{0};

  int xshift = bitops::fls64(poly) - 1;
  u_int64_t T1 = polymod(0, INT64(1) << xshift, poly);
  for (uint_fast16_t j = 0; j < 256; j++) {
    ret[j] = polymmult(j, T1, poly) | ((u_int64_t)j << xshift);
  }
  return ret;
}

constexpr std::array<uint64_t, 256> calcU(const std::array<uint64_t, 256> &T) {
  std::array<uint64_t, 256> U{0};
  uint64_t shift_size = 1;
  for (uint_fast16_t i = 1; i < 256; i++) shift_size = append8(shift_size, 0, T);
  for (uint_fast16_t i = 0; i < 256; i++) U[i] = polymmult(i, shift_size, poly);
  return U;
}

/**
 * Static generator of the polynomial.
 */
constexpr const static std::array<uint64_t, 256> T = calcT();
constexpr const static std::array<uint64_t, 256> U = calcU(T);

uint64_t rolling_checksum(uint64_t csum, std::byte c1, std::byte c2) {
  return append8(csum ^ U[static_cast<uint8_t>(c1)], static_cast<unsigned char>(c2), T);
}

/*
 *   a simple 32 bit checksum that can be upadted from end
 */
uint64_t checksum(std::string &buf) {
  uint64_t sum = 0;
  for (auto c : buf) {
    sum = rolling_checksum(sum, static_cast<std::byte>(0), static_cast<std::byte>(c));
  }
  return sum;
}

}  //   namespace P2PFileSync::RabinChecksum