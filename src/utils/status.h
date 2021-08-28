/*
 * @Author: Hanze Chen
 * @Date: 2021-08-27 20:35:52
 * @Last Modified by: Hanze Chen
 * @Last Modified time: 2021-08-27 20:36:20
 */

#ifndef P2P_FILE_SYNC_UTILS_STATUS_H
#define P2P_FILE_SYNC_UTILS_STATUS_H

#include <cstring>
#include <string>

namespace P2PFileSync {
enum __attribute__((__packed__)) StatusCode {
  /**
   * Status which means no error, no exception, preform exactly same
   * behaviors as method described
   */
  OK = 1,

  /**
   * Status which means current method or parameter combination are not
   * supported or even not implemented
   */
  UNAVAILABLE = 2,

  /**
   * Sataus which means current method argument are incorrect
   *
   */
  INVALID_ARGUMENT = 3,
};

class Status {
 public:
  Status(StatusCode code) : Status(code, "No message Provided") {}

  Status(StatusCode code, std::string &&msg) {
    this->status_code_ = code;
    this->msg_ = std::move(msg);
  }

  StatusCode Code() { return status_code_; }
  std::string Message() const { return this->msg_; }

  static const Status OK() {
    return Status(StatusCode::OK, "Return Ok without error!");
  }

 private:
  std::string msg_;
  StatusCode status_code_;
};
}  // namespace P2PFileSync

#endif