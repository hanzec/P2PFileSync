//
// Created by hanzech on 11/27/21.
//
#include <gtest/gtest.h>
#include "../base64_utils.h"

TEST(UTILS_TEST, BASE64_TEST_ENCODE_1) {
  std::string input = "Hello World!";
    std::string output = "SGVsbG8gV29ybGQh";
    EXPECT_EQ(output, P2PFileSync::ServerKit::Base64::encode(input));
}

TEST(UTILS_TEST, BASE64_TEST_ENCODE_2) {
  std::string input = "This is a really long string that will be encoded";
  std::string output = "VGhpcyBpcyBhIHJlYWxseSBsb25nIHN0cmluZyB0aGF0IHdpbGwgYmUgZW5jb2RlZA==";
  EXPECT_EQ(output, P2PFileSync::ServerKit::Base64::encode(input));
}

TEST(UTILS_TEST, BASE64_TEST_DECODE_1) {
  std::string input = "Hello World!";
  std::string output = "SGVsbG8gV29ybGQh";
  EXPECT_EQ(input, P2PFileSync::ServerKit::Base64::decode(output));
}

TEST(UTILS_TEST, BASE64_TEST_DECODE_2) {
  std::string input = "This is a really long string that will be encoded";
  std::string output = "VGhpcyBpcyBhIHJlYWxseSBsb25nIHN0cmluZyB0aGF0IHdpbGwgYmUgZW5jb2RlZA==";
  EXPECT_EQ(input, P2PFileSync::ServerKit::Base64::decode(output));
}
