#include "base64_utils.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
namespace P2PFileSync::Serverkit::Base64 {
static const constexpr char alphabet_map[] ={
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

static constexpr unsigned char kDecodingTable[] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

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

std::string decode(const std::string& input) {
  std::stringstream os;
  size_t i;
  for (i = 0; i + 4 <= input.length(); i += 4) {
    uint32_t c0 = kDecodingTable[static_cast<uint8_t>(input[i])];
    uint32_t c1 = kDecodingTable[static_cast<uint8_t>(input[i + 1])];
    uint32_t c2 = kDecodingTable[static_cast<uint8_t>(input[i + 2])];
    uint32_t c3 = kDecodingTable[static_cast<uint8_t>(input[i + 3])];


    uint32_t tmp = (c0 << 2) | (c1 >> 4);
    os << static_cast<char>(tmp);

    if (c2 != 64) {
      tmp = ((c1 & 0x0f) << 4) | (c2 >> 2);
      os << static_cast<char>(tmp);
    }

    if (c3 != 64) {
      tmp = ((c2 & 0x03) << 6) | c3;
      os << static_cast<char>(tmp);
    }
  }
  return os.str();
}

}  // namespace P2PFileSync::Base64