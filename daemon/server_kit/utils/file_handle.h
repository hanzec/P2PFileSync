//
// Created by hanzech on 11/14/21.
//

#ifndef P2P_FILE_SYNC_FILE_HANDLE_H
#define P2P_FILE_SYNC_FILE_HANDLE_H
#include <filesystem>

namespace P2PFileSync::Serverkit {
class FIleHandle{
 public:
  std::shared_ptr<FIleHandle> new_handler(std::filesystem::path path, const char* read_mode);

 protected:
  FIleHandle(FILE * fp);

 private:
  FILE * _fp;
};

}; // P2PFileSync::Serverki
#endif  // P2P_FILE_SYNC_FILE_HANDLE_H
