#include "utils/ip_address.h"
#include <gtest/gtest.h>

TEST(UTILS_TEST, ip_addr_normal) {
  P2PFileSync::IPAddress ip("100.100.100.1:1234");

  // test valid
  ASSERT_TRUE(ip.valid());

  // test get port success
  ASSERT_EQ(ip.port(), 1234);

  // test debug string
  std::stringstream ss;
  ss << ip;
  ASSERT_EQ(ss.str(), "100.100.100.1:1234") << "debug string error ";
}

TEST(UTILS_TEST, ip_addr_not_valid) {
  {
    P2PFileSync::IPAddress ip("2990.100.100.1:1234");
    ASSERT_FALSE(ip.valid()) << "2990.100.100.1:1234";
  }
  {
    P2PFileSync::IPAddress ip("1.00.100.1.1:1234");
    ASSERT_FALSE(ip.valid()) << "1.00.100.1:1234";
  }

  {
    P2PFileSync::IPAddress ip("1.1.9999999999999.1:1234");
    ASSERT_FALSE(ip.valid()) << "1.1.100.1:1234";
  }

  {
    P2PFileSync::IPAddress ip("1.1.100.1:");
    ASSERT_FALSE(ip.valid()) << "1.1.100.1:";
  }
}

TEST(UTILS_TEST, ip_addr_bad_input) {
  {
    P2PFileSync::IPAddress ip("not valid string");
    ASSERT_FALSE(ip.valid()) << "not valid string";
  }

  {
    P2PFileSync::IPAddress ip("");
    ASSERT_FALSE(ip.valid()) << "empty string";
  }

  {
    P2PFileSync::IPAddress ip("1avc.123.das.sd1:asd");
    ASSERT_FALSE(ip.valid()) << "1avc.123.das.sd1:asd";
  }
}
