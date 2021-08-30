#ifndef P2P_FILE_SYNC_MANAGE_UTILS_COMMAND_UTILS_H
#define P2P_FILE_SYNC_MANAGE_UTILS_COMMAND_UTILS_H

namespace P2PFileSync {
  using command_type_t = enum {
    front_operation, change_directory, exit_shell, clean_screen
  };

  using command_t = struct command{
    char * exec;
    char * file_name;
    char * * argument;
    int background_task;

    command_type_t command_type;
    struct command * next_command;
};
} // P2PFileSync

#endif