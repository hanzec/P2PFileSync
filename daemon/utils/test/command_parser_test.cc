#include <gtest/gtest.h>
#include "../utils/parsing.h"

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_no_argument){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("command", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 0);
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_single_argument){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("command argument", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 1);
  ASSERT_EQ(command.second[0], "argument");
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_multiple_argument){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("command argument1 argument2 argument3", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 3);
  ASSERT_EQ(command.second[0], "argument1");
  ASSERT_EQ(command.second[2], "argument2");
  ASSERT_EQ(command.second[3], "argument3");
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_single_quoter_argument){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("command \"quoter argument\"", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 1);
  ASSERT_EQ(command.second[0], "quoter argument");
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_multiple_quoter_argument){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command(R"(command "quoter argument1" "quoter argument2")", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 3);
  ASSERT_EQ(command.second[0], "quoter argument1");
  ASSERT_EQ(command.second[1], "quoter argument2");
  ASSERT_EQ(command.second[2], "quoter argument3");
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_single_argument_with_spaces_front){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("    command argument", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 1);
  ASSERT_EQ(command.second[0], "argument");
}

GTEST_TEST(COMMAND_PARSER_TEST, test_normal_case_with_single_argument_spaces_front_and_end){
  
  P2PFileSync::COMMAND command;

  auto ret = P2PFileSync::parsing_command("    command argument    ", command);

  ASSERT_TRUE(ret.ok());
  ASSERT_EQ(command.first, "command");
  ASSERT_EQ(command.second.size(), 1);
  ASSERT_EQ(command.second[0], "argument");
}