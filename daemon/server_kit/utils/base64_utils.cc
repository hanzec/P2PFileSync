#include "base64_utils.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
namespace P2PFileSync::Serverkit::Base64 {
static const constexpr std::array<uint8_t, 64> alphabet_map{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

static const constexpr std::array<uint8_t, 128> reverse_map{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255,
    255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255,
    255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
    10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
    25,  255, 255, 255, 255, 255, 255, 26,  27,  28,  29,  30,  31,  32,  33,
    34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51,  255, 255, 255, 255, 255};

std::string encode(const std::string& text) {
  std::ostringstream os;

  size_t i, j;
  for (i = 0, j = 0; i + 3 <= text.length(); i += 3, j += 4) {
    os << alphabet_map[text[i] >> 2]
       << alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)]
       << alphabet_map[((text[i + 1] << 2) & 0x3c) | (text[i + 2] >> 6)]
       << alphabet_map[text[i + 2] & 0x3f];
  }

  if (i < text.length()) {
    uint32_t tail = text.length() - i;
    if (tail == 1) {
      os << alphabet_map[text[i] >> 2] << alphabet_map[(text[i] << 4) & 0x30]
         << "==";
    } else {
      os << alphabet_map[text[i] >> 2]
         << alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)]
         << alphabet_map[(text[i + 1] << 2) & 0x3c] << '=';
    }
  }
  return os.str();
}

std::string decode(const std::string& text) {
  assert((text.length() & 0x03) == 0);

  std::ostringstream os;

  size_t i;
  for (i = 0; i < text.length(); i += 4) {
    std::array<uint8_t, 4> quad{reverse_map[text[i]], 
                                reverse_map[text[i + 1]],
                                reverse_map[text[i + 2]],
                                reverse_map[text[i + 3]]};

    assert(quad[0] < 64 && quad[1] < 64);

    os << static_cast<char>((quad[0] << 2) | (quad[1] >> 4));

    if (quad[2] >= 64) {
      break;
    } else if (quad[3] >= 64) {
      os << static_cast<char>((quad[1] << 4) | (quad[2] >> 2))
         << static_cast<char>((quad[1] << 4) | (quad[2] >> 2))
         << static_cast<char>((quad[2] << 6) | quad[3]);
    }
  }
  return os.str();
}
}  // namespace P2PFileSync::Base64