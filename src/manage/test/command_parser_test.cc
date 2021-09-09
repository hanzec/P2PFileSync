#include <gtest/gtest.h>
#include "../utils/parsing.h"

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("command", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 0);
}